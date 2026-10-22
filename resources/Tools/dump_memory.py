#!/usr/bin/env python3
"""
Evitania Live Process Memory & Module Dumper
Extracts unpacked/decrypted GameAssembly.dll and global-metadata.dat directly from
process RAM (supporting Linux/Wine/Proton and native Windows).

Solves:
1. Encrypted / Packed GameAssembly.dll on disk.
2. Encrypted / Stripped global-metadata.dat.
3. Produces a valid PE on disk with RVA==FileOffset for instant analysis.
"""

import argparse
import ctypes
import os
import platform
import re
import struct
import sys
import time

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../.."))
DEFAULT_OUTPUT_DIR = os.path.join(PROJECT_ROOT, "resources", "dumped")

IL2CPP_METADATA_MAGIC = b"\xaf\x1b\xb1\xfa"  # 0xFAB11BAF in Little Endian


def print_banner(title):
    print("\n" + "=" * 65)
    print(f"  {title.center(61)}")
    print("=" * 65)


# ==============================================================================
# Linux / Wine / Proton Process & Memory Introspection
# ==============================================================================
def find_pids_linux(process_pattern):
    pids = []
    pattern_re = re.compile(process_pattern, re.IGNORECASE)

    try:
        for entry in os.listdir("/proc"):
            if not entry.isdigit():
                continue
            pid = int(entry)
            try:
                # Check comm
                comm_path = f"/proc/{pid}/comm"
                with open(comm_path, "r", encoding="utf-8", errors="ignore") as f:
                    comm = f.read().strip()
                if pattern_re.search(comm):
                    pids.append((pid, comm))
                    continue

                # Check cmdline
                cmdline_path = f"/proc/{pid}/cmdline"
                with open(cmdline_path, "rb") as f:
                    cmdline_parts = (
                        f.read().decode("utf-8", errors="ignore").split("\x00")
                    )
                exe = cmdline_parts[0] if cmdline_parts else ""
                exe_name = os.path.basename(exe.replace("\\", "/"))
                # Match against executable name or any .exe binary argument (e.g. Wine/Proton launching EvitaniaOnline.exe)
                if pattern_re.search(exe_name) or any(
                    pattern_re.search(os.path.basename(arg.replace("\\", "/")))
                    and arg.lower().endswith(".exe")
                    for arg in cmdline_parts
                ):
                    pids.append((pid, exe if exe else comm))
            except (FileNotFoundError, PermissionError):
                continue
    except Exception as e:
        print(f"[-] Error scanning /proc: {e}")

    return pids


def get_process_maps_linux(pid):
    """
    Parses /proc/<pid>/maps.
    Returns list of dicts:
      [{'start': int, 'end': int, 'perms': str, 'offset': int, 'dev': str, 'inode': int, 'path': str}]
    """
    maps = []
    maps_path = f"/proc/{pid}/maps"
    try:
        with open(maps_path, "r", encoding="utf-8", errors="ignore") as f:
            for line in f:
                parts = line.strip().split(maxsplit=5)
                if len(parts) < 5:
                    continue
                addr_range = parts[0].split("-")
                start = int(addr_range[0], 16)
                end = int(addr_range[1], 16)
                perms = parts[1]
                offset = int(parts[2], 16)
                path = parts[5] if len(parts) > 5 else ""
                maps.append(
                    {
                        "start": start,
                        "end": end,
                        "perms": perms,
                        "offset": offset,
                        "path": path,
                    }
                )
    except Exception as e:
        print(f"[-] Error reading {maps_path}: {e}")
    return maps


def read_process_memory_linux(pid, address, size):
    mem_path = f"/proc/{pid}/mem"
    try:
        with open(mem_path, "rb") as f:
            f.seek(address)
            return f.read(size)
    except Exception as e:
        # Fallback to pread if open/seek fails
        try:
            fd = os.open(mem_path, os.O_RDONLY)
            try:
                return os.pread(fd, size, address)
            finally:
                os.close(fd)
        except Exception:
            return None


# ==============================================================================
# Windows Process & Memory Introspection (ctypes)
# ==============================================================================
def find_pids_windows(process_pattern):
    pids = []
    if platform.system() != "Windows":
        return pids

    pattern_re = re.compile(process_pattern, re.IGNORECASE)
    kernel32 = ctypes.windll.kernel32

    TH32CS_SNAPPROCESS = 0x00000002

    class PROCESSENTRY32(ctypes.Structure):
        _fields_ = [
            ("dwSize", ctypes.c_ulong),
            ("cntUsage", ctypes.c_ulong),
            ("th32ProcessID", ctypes.c_ulong),
            ("th32DefaultHeapID", ctypes.POINTER(ctypes.c_ulong)),
            ("th32ModuleID", ctypes.c_ulong),
            ("cntThreads", ctypes.c_ulong),
            ("th32ParentProcessID", ctypes.c_ulong),
            ("pcPriClassBase", ctypes.c_long),
            ("dwFlags", ctypes.c_ulong),
            ("szExeFile", ctypes.c_char * 260),
        ]

    snapshot = kernel32.CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
    if snapshot == -1:
        return pids

    entry = PROCESSENTRY32()
    entry.dwSize = ctypes.sizeof(PROCESSENTRY32)

    if kernel32.Process32First(snapshot, ctypes.byref(entry)):
        while True:
            exe_name = entry.szExeFile.decode("latin-1", errors="ignore")
            if pattern_re.search(exe_name):
                pids.append((entry.th32ProcessID, exe_name))
            if not kernel32.Process32Next(snapshot, ctypes.byref(entry)):
                break

    kernel32.CloseHandle(snapshot)
    return pids


def read_process_memory_windows(h_process, address, size):
    kernel32 = ctypes.windll.kernel32
    buffer = ctypes.create_string_buffer(size)
    bytes_read = ctypes.c_size_t(0)
    success = kernel32.ReadProcessMemory(
        h_process,
        ctypes.c_void_p(address),
        buffer,
        ctypes.c_size_t(size),
        ctypes.byref(bytes_read),
    )
    if success and bytes_read.value > 0:
        return buffer.raw[: bytes_read.value]
    return None


# ==============================================================================
# PE Header Reconstruction (Mapped Memory -> Disk-Valid PE)
# ==============================================================================
def fix_pe_section_headers(pe_bytes):
    """
    Fixes the Section Headers of a memory-dumped PE so that:
      PointerToRawData = VirtualAddress
      SizeOfRawData    = VirtualSize (aligned)
    This aligns disk offsets with RVAs, making the dumped PE directly readable by
    Capstone, Ghidra, IDA, and hex analyzers without needing complex unmapping.
    """
    if len(pe_bytes) < 0x200:
        return pe_bytes

    (e_magic,) = struct.unpack("<H", pe_bytes[:2])
    if e_magic != 0x5A4D:
        return pe_bytes

    (e_lfanew,) = struct.unpack("<I", pe_bytes[0x3C:0x40])
    if e_lfanew + 0x100 > len(pe_bytes):
        return pe_bytes

    pe_sig = pe_bytes[e_lfanew : e_lfanew + 4]
    if pe_sig != b"PE\x00\x00":
        return pe_bytes

    (num_sections,) = struct.unpack("<H", pe_bytes[e_lfanew + 6 : e_lfanew + 8])
    (opt_size,) = struct.unpack("<H", pe_bytes[e_lfanew + 20 : e_lfanew + 22])
    sec_table_offset = e_lfanew + 24 + opt_size

    mutable = bytearray(pe_bytes)

    for i in range(num_sections):
        sec_offset = sec_table_offset + i * 40
        if sec_offset + 40 > len(mutable):
            break

        sec_data = mutable[sec_offset : sec_offset + 40]
        vsize, vaddr, raw_size, raw_ptr = struct.unpack("<IIII", sec_data[8:24])

        # Align raw_size to 0x200
        aligned_size = (vsize + 0x1FF) & ~0x1FF

        # Rewrite PointerToRawData = VirtualAddress, SizeOfRawData = aligned VirtualSize
        struct.pack_into("<II", mutable, sec_offset + 16, aligned_size, vaddr)

    return bytes(mutable)


# ==============================================================================
# Main Dumper Engine
# ==============================================================================
def dump_from_linux(pid, module_name, output_dir, fix_pe=True):
    print(f"[*] Attaching to PID {pid} via Linux procfs...")
    maps = get_process_maps_linux(pid)
    if not maps:
        print(f"[-] No memory mappings found for PID {pid}")
        return False

    # 1. Locate Target Module (e.g. GameAssembly.dll)
    mod_regions = [m for m in maps if module_name.lower() in m["path"].lower()]
    if not mod_regions:
        print(f"[-] Module '{module_name}' not found in /proc/{pid}/maps.")
        print("    Available loaded modules:")
        loaded_so = set(
            os.path.basename(m["path"])
            for m in maps
            if m["path"] and m["path"].endswith((".dll", ".so"))
        )
        for m in sorted(loaded_so):
            print(f"      - {m}")
        return False

    base_addr = min(m["start"] for m in mod_regions)
    max_addr = max(m["end"] for m in mod_regions)
    total_size = max_addr - base_addr

    print(f"[+] Found {module_name} in RAM:")
    print(f"    - Base Address: 0x{base_addr:X}")
    print(f"    - End Address:  0x{max_addr:X}")
    print(
        f"    - Mapped Size:  {total_size / (1024 * 1024):.2f} MB ({total_size} bytes)"
    )

    # 2. Read PE Module from Memory (Segment-by-Segment into a contiguous image)
    print(
        f"[*] Reading module memory space across {len(mod_regions)} mapped segments..."
    )
    raw_pe_buf = bytearray(total_size)
    bytes_read_total = 0

    for seg in mod_regions:
        seg_start = seg["start"]
        seg_size = seg["end"] - seg_start
        seg_offset = seg_start - base_addr
        if "r" in seg["perms"]:
            chunk = read_process_memory_linux(pid, seg_start, seg_size)
            if chunk:
                raw_pe_buf[seg_offset : seg_offset + len(chunk)] = chunk
                bytes_read_total += len(chunk)

    if bytes_read_total < 0x1000 or raw_pe_buf[:2] != b"MZ":
        print(
            f"[-] Failed to read valid PE image from /proc/{pid}/mem (Read {bytes_read_total} bytes)."
        )
        return False

    print(
        f"[+] Successfully read {bytes_read_total / (1024 * 1024):.2f} MB across all valid segments."
    )
    print("[+] Validated PE MZ header in RAM.")
    raw_pe = bytes(raw_pe_buf)

    # 3. Apply PE Fixup
    if fix_pe:
        print("[*] Fixing PE section headers for disk validity (RVA == FileOffset)...")
        final_pe = fix_pe_section_headers(raw_pe)
    else:
        final_pe = raw_pe

    os.makedirs(output_dir, exist_ok=True)
    out_dll_path = os.path.join(output_dir, "GameAssembly_memory.dll")
    with open(out_dll_path, "wb") as f:
        f.write(final_pe)
    print(f"[+] Successfully dumped decrypted module -> {out_dll_path}")

    # 4. Attempt to find and dump global-metadata.dat
    print("\n[*] Searching memory for global-metadata.dat...")
    metadata_regions = [m for m in maps if "global-metadata.dat" in m["path"].lower()]
    if metadata_regions:
        meta_start = min(m["start"] for m in metadata_regions)
        meta_end = max(m["end"] for m in metadata_regions)
        meta_size = meta_end - meta_start
        print(
            f"[+] Found mapped global-metadata.dat at 0x{meta_start:X} ({meta_size} bytes)"
        )
        meta_data = read_process_memory_linux(pid, meta_start, meta_size)
        if meta_data and meta_data[:4] == IL2CPP_METADATA_MAGIC:
            out_meta_path = os.path.join(output_dir, "global-metadata_dumped.dat")
            with open(out_meta_path, "wb") as f:
                f.write(meta_data)
            print(f"[+] Successfully dumped metadata -> {out_meta_path}")
        else:
            print("[-] Mapped metadata region did not match magic 0xFAB11BAF.")
    else:
        # Scan readable regions for IL2CPP metadata magic 0xFAB11BAF
        print("[*] Performing memory sweep for 0xFAB11BAF metadata signature...")
        found_meta = False
        for m in maps:
            if "r" in m["perms"] and (m["end"] - m["start"]) >= 0x10000:
                header = read_process_memory_linux(pid, m["start"], 0x40)
                if header and header[:4] == IL2CPP_METADATA_MAGIC:
                    # Header matches! Read metadata header to find total length
                    # struct Il2CppGlobalMetadataHeader: offset 0x08 is stringLiteralOffset or definitionsOffset
                    (version,) = struct.unpack("<I", header[4:8])
                    print(
                        f"[+] Found unmapped decrypted metadata at 0x{m['start']:X} (Version {version})!"
                    )
                    meta_dump = read_process_memory_linux(
                        pid, m["start"], m["end"] - m["start"]
                    )
                    if meta_dump:
                        out_meta_path = os.path.join(
                            output_dir, "global-metadata_dumped.dat"
                        )
                        with open(out_meta_path, "wb") as f:
                            f.write(meta_dump)
                        print(f"[+] Successfully dumped metadata -> {out_meta_path}")
                        found_meta = True
                        break
        if not found_meta:
            print(
                "[*] Metadata not isolated in swept memory regions (may be embedded in data sections)."
            )

    return True


def dump_from_windows(pid, module_name, output_dir, fix_pe=True):
    if platform.system() != "Windows":
        print("[-] Windows dumper called on non-Windows OS.")
        return False

    print(f"[*] Attaching to PID {pid} via Win32 API...")
    kernel32 = ctypes.windll.kernel32
    psapi = ctypes.windll.psapi

    PROCESS_QUERY_INFORMATION = 0x0400
    PROCESS_VM_READ = 0x0010
    h_process = kernel32.OpenProcess(
        PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, False, pid
    )
    if not h_process:
        print(
            f"[-] OpenProcess failed for PID {pid} (Error {kernel32.GetLastError()}). Run as Administrator."
        )
        return False

    try:
        # Enum modules
        h_mods = (ctypes.c_void_p * 1024)()
        cb_needed = ctypes.c_ulong(0)
        if not psapi.EnumProcessModules(
            h_process,
            ctypes.byref(h_mods),
            ctypes.sizeof(h_mods),
            ctypes.byref(cb_needed),
        ):
            print(f"[-] EnumProcessModules failed.")
            return False

        count = cb_needed.value // ctypes.sizeof(ctypes.c_void_p)
        target_base = None

        class MODULEINFO(ctypes.Structure):
            _fields_ = [
                ("lpBaseOfDll", ctypes.c_void_p),
                ("SizeOfImage", ctypes.c_ulong),
                ("EntryPoint", ctypes.c_void_p),
            ]

        for i in range(count):
            h_mod = h_mods[i]
            name_buf = ctypes.create_string_buffer(260)
            psapi.GetModuleBaseNameA(
                h_process, h_mod, name_buf, ctypes.sizeof(name_buf)
            )
            mod_name = name_buf.value.decode("latin-1", errors="ignore")
            if module_name.lower() in mod_name.lower():
                mod_info = MODULEINFO()
                psapi.GetModuleInformation(
                    h_process, h_mod, ctypes.byref(mod_info), ctypes.sizeof(mod_info)
                )
                target_base = mod_info.lpBaseOfDll
                target_size = mod_info.SizeOfImage
                print(
                    f"[+] Found {mod_name} at base 0x{target_base:X} (Size: {target_size} bytes)"
                )
                break

        if not target_base:
            print(f"[-] Module '{module_name}' not loaded in process.")
            return False

        raw_pe = read_process_memory_windows(h_process, target_base, target_size)
        if not raw_pe:
            print("[-] ReadProcessMemory failed.")
            return False

        if fix_pe:
            final_pe = fix_pe_section_headers(raw_pe)
        else:
            final_pe = raw_pe

        os.makedirs(output_dir, exist_ok=True)
        out_dll_path = os.path.join(output_dir, "GameAssembly_memory.dll")
        with open(out_dll_path, "wb") as f:
            f.write(final_pe)
        print(f"[+] Successfully dumped decrypted module -> {out_dll_path}")
        return True

    finally:
        kernel32.CloseHandle(h_process)


def main():
    parser = argparse.ArgumentParser(
        description="Evitania Live Process Memory & Module Dumper"
    )
    parser.add_argument(
        "-p", "--pid", type=int, default=None, help="Process ID of the game"
    )
    parser.add_argument(
        "-n",
        "--name",
        default="Evitania",
        help="Process name pattern to match (default: Evitania)",
    )
    parser.add_argument(
        "-m",
        "--module",
        default="GameAssembly.dll",
        help="Target module name to dump (default: GameAssembly.dll)",
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        default=DEFAULT_OUTPUT_DIR,
        help=f"Directory to save dumped binaries (default: {DEFAULT_OUTPUT_DIR})",
    )
    parser.add_argument(
        "--no-fix-pe",
        action="store_true",
        help="Do not fix PE section headers (keep raw virtual layout)",
    )

    args = parser.parse_args()

    print_banner("EVITANIA LIVE PROCESS MEMORY DUMPER")

    pid = args.pid
    proc_name = args.name

    if pid is None:
        print(f"[*] Searching for running process matching '{proc_name}'...")
        if platform.system() == "Linux":
            matches = find_pids_linux(proc_name)
        else:
            matches = find_pids_windows(proc_name)

        if not matches:
            print(f"[-] No running process matching '{proc_name}' found.")
            print("[*] Please launch the game first, then re-run this tool.")
            sys.exit(1)

        if len(matches) == 1:
            pid, found_name = matches[0]
            print(f"[+] Auto-detected game process: PID {pid} ({found_name})")
        else:
            print(f"[!] Multiple matching processes found:")
            for p, name in matches:
                print(f"    - PID {p}: {name}")
            pid = matches[0][0]
            print(f"[*] Selecting first candidate PID {pid}...")

    # Execute Dumper
    if platform.system() == "Linux":
        success = dump_from_linux(
            pid=pid,
            module_name=args.module,
            output_dir=args.output_dir,
            fix_pe=not args.no_fix_pe,
        )
    else:
        success = dump_from_windows(
            pid=pid,
            module_name=args.module,
            output_dir=args.output_dir,
            fix_pe=not args.no_fix_pe,
        )

    if success:
        print("\n" + "=" * 65)
        print("  DUMP COMPLETE - READY FOR SCANNING & EXTRACTION")
        print("  Use `--dll resources/dumped/GameAssembly_memory.dll` with updater")
        print("=" * 65)
        sys.exit(0)
    else:
        print_banner("DUMP FAILED")
        sys.exit(1)


if __name__ == "__main__":
    main()
