#!/usr/bin/env python3

"""
MIT License

Copyright © 2026 - Present RiiK

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

"""
Evitania Unified Update Runner
Automates the end-to-end update workflow:
1. Detects Steam updates (build ID & assembly SHA-256)
2. Scans and validates AOB signatures & semantic offsets
3. Re-baselines config.json metadata
4. Invokes the project build script to produce release binaries
5. Self-heals on failure (RAM dump, dump.cs regeneration, signature DB rebuild,
   rollback) and diagnoses with the doctor (`--doctor`)
"""

import argparse
import hashlib
import json
import os
import platform
import re
import shutil
import struct
import subprocess
import sys
import time

# Add Tools directory to sys.path
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../.."))
sys.path.insert(0, SCRIPT_DIR)

from aobgenerator import find_game_assembly, generate_signatures_and_offsets
from dump_memory import find_pids_linux, find_pids_windows
import doctor

CONFIG_PATH = os.path.join(PROJECT_ROOT, "config.json")
APP_ID = "4119420"


def get_file_sha256(filepath):
    hasher = hashlib.sha256()
    with open(filepath, "rb") as f:
        while chunk := f.read(65536):
            hasher.update(chunk)
    return hasher.hexdigest()


def find_steam_manifest(dll_path):
    if not dll_path or not os.path.exists(dll_path):
        return None

    # Expected structure: <SteamLibrary>/steamapps/common/<GameFolder>/GameAssembly.dll
    # Manifest is at:    <SteamLibrary>/steamapps/appmanifest_4119420.acf
    current = os.path.dirname(os.path.abspath(dll_path))
    for _ in range(3):
        parent = os.path.dirname(current)
        if (
            os.path.basename(current).lower() == "common"
            and os.path.basename(parent).lower() == "steamapps"
        ):
            manifest = os.path.join(parent, f"appmanifest_{APP_ID}.acf")
            if os.path.exists(manifest):
                return manifest
        current = parent
    return None


def read_manifest_info(manifest_path):
    info = {"buildid": "unknown", "LastUpdated": "unknown", "installdir": "unknown"}
    if not manifest_path or not os.path.exists(manifest_path):
        return info

    try:
        with open(manifest_path, "r", encoding="utf-8") as f:
            content = f.read()

        for key in info.keys():
            match = re.search(rf'"{key}"\s+"([^"]+)"', content, re.IGNORECASE)
            if match:
                info[key] = match.group(1)
    except Exception:
        pass
    return info


def _lz4_decompress_block(src):
    """Minimal pure-Python LZ4 raw block decompressor (for UnityFS bundles)."""
    out = bytearray()
    i, n = 0, len(src)
    while i < n:
        token = src[i]
        i += 1
        lit_len = token >> 4
        if lit_len == 15:
            while i < n:
                b = src[i]
                i += 1
                lit_len += b
                if b != 255:
                    break
        out.extend(src[i : i + lit_len])
        i += lit_len
        if i >= n:
            break
        offset = src[i] | (src[i + 1] << 8)
        i += 2
        if offset <= 0 or offset > len(out):
            break
        match_len = (token & 0x0F) + 4
        if match_len == 19:
            while i < n:
                b = src[i]
                i += 1
                match_len += b
                if b != 255:
                    break
        start = len(out) - offset
        for j in range(match_len):
            out.append(out[start + j])
    return bytes(out)


def _read_globalgamemanagers_head(data_dir, max_bytes=65536):
    """Return the first `max_bytes` of Unity's globalgamemanagers asset."""
    loose = os.path.join(data_dir, "globalgamemanagers")
    if os.path.isfile(loose):
        with open(loose, "rb") as f:
            return f.read(max_bytes)

    bundle = os.path.join(data_dir, "data.unity3d")
    if not os.path.isfile(bundle):
        return None

    def _read_cstr(fh):
        buf = bytearray()
        while True:
            ch = fh.read(1)
            if not ch or ch == b"\x00":
                return bytes(buf)
            buf.extend(ch)

    with open(bundle, "rb") as f:
        if _read_cstr(f) != b"UnityFS":
            return None
        ver = struct.unpack(">I", f.read(4))[0]
        _read_cstr(f)  # unity version
        _read_cstr(f)  # unity revision
        size, c_blocks_size, u_blocks_size, flags = struct.unpack(">QIII", f.read(20))
        if ver >= 7:
            pad = (16 - (f.tell() % 16)) % 16
            f.read(pad)
        if flags & 0x80:
            data_pos = f.tell()
            f.seek(size - c_blocks_size)
            raw_blocks_info = f.read(c_blocks_size)
        else:
            raw_blocks_info = f.read(c_blocks_size)
            data_pos = f.tell()
            if flags & 0x200:
                data_pos = (data_pos + 15) & ~15

        comp = flags & 0x3F
        if comp in (2, 3):
            blocks_info = _lz4_decompress_block(raw_blocks_info)
        elif comp == 0:
            blocks_info = raw_blocks_info
        else:
            return None

        pos = 16  # skip 16-byte uncompressedDataHash
        num_blocks = struct.unpack_from(">I", blocks_info, pos)[0]
        pos += 4
        blocks = []
        for _ in range(num_blocks):
            blocks.append(struct.unpack_from(">IIH", blocks_info, pos))
            pos += 10
        num_nodes = struct.unpack_from(">I", blocks_info, pos)[0]
        pos += 4
        ggm_range = None
        for _ in range(num_nodes):
            off, sz, _ = struct.unpack_from(">QQI", blocks_info, pos)
            pos += 20
            end = blocks_info.index(b"\x00", pos)
            name = blocks_info[pos:end].decode("utf-8", "ignore")
            pos = end + 1
            if name == "globalgamemanagers":
                ggm_range = (off, min(sz, max_bytes))
                break

        if not ggm_range:
            return None

        g_off, g_len = ggm_range
        cur_u = 0
        cur_c = data_pos
        out = bytearray()
        for u_sz, c_sz, b_flags in blocks:
            if cur_u + u_sz > g_off and cur_u < g_off + g_len:
                f.seek(cur_c)
                raw = f.read(c_sz)
                bc = b_flags & 0x3F
                if bc in (2, 3):
                    dec = _lz4_decompress_block(raw)
                elif bc == 0:
                    dec = raw
                else:
                    return None
                s = max(0, g_off - cur_u)
                e = min(u_sz, g_off + g_len - cur_u)
                out.extend(dec[s:e])
            cur_u += u_sz
            cur_c += c_sz
            if cur_u >= g_off + g_len:
                break
        return bytes(out)


def detect_game_version(dll_path):
    """Extract the real Unity PlayerSettings bundleVersion next to GameAssembly.dll."""
    if not dll_path:
        return None
    game_dir = os.path.dirname(os.path.abspath(dll_path))
    if not os.path.isdir(game_dir):
        return None

    data_dir = None
    for entry in os.listdir(game_dir):
        candidate = os.path.join(game_dir, entry)
        if entry.endswith("_Data") and os.path.isdir(candidate):
            data_dir = candidate
            break
    if not data_dir:
        return None

    anchors = []
    app_info = os.path.join(data_dir, "app.info")
    if os.path.isfile(app_info):
        try:
            with open(app_info, "r", encoding="utf-8", errors="ignore") as f:
                lines = [ln.strip() for ln in f.read().splitlines() if ln.strip()]
            # Prefer productName (line 1) then companyName (line 0)
            for ln in reversed(lines[:2]):
                anchors.append(ln.encode("utf-8"))
        except Exception:
            pass

    try:
        ggm = _read_globalgamemanagers_head(data_dir)
    except Exception:
        return None
    if not ggm:
        return None

    start_idx = -1
    for anchor in anchors:
        idx = ggm.find(anchor)
        if idx != -1:
            start_idx = idx + len(anchor)
            break
    if start_idx == -1:
        start_idx = 0

    # Align to 4-byte boundary and scan PlayerSettings for a length-prefixed X.Y.Z string
    scan_start = (start_idx + 3) & ~3
    scan_end = min(len(ggm) - 8, scan_start + 2048)
    ver_re = re.compile(rb"^\d+\.\d+\.\d+(?:[-+._a-zA-Z0-9]*)?$")
    for pos in range(scan_start, scan_end, 4):
        str_len = struct.unpack_from("<I", ggm, pos)[0]
        if 3 <= str_len <= 32 and pos + 4 + str_len <= len(ggm):
            raw_s = ggm[pos + 4 : pos + 4 + str_len]
            if ver_re.match(raw_s):
                pad_len = (4 - (str_len % 4)) % 4
                if all(
                    b == 0 for b in ggm[pos + 4 + str_len : pos + 4 + str_len + pad_len]
                ):
                    return raw_s.decode("ascii")
    return None


def print_banner(title):
    print("\n" + "=" * 65)
    print(f"  {title.center(61)}")
    print("=" * 65)


def run_build(build_type):
    print(f"\n[*] Triggering project build [{build_type}]...")
    is_windows = platform.system() == "Windows"
    if is_windows:
        build_script = os.path.join(PROJECT_ROOT, "scripts", "build.bat")
        cmd = [build_script, build_type]
    else:
        build_script = os.path.join(PROJECT_ROOT, "scripts", "build")
        cmd = ["bash", build_script, build_type]

    if not os.path.exists(build_script):
        print(f"[-] Error: Build runner not found at {build_script}")
        return False

    # Stream output live while capturing it for doctor diagnosis.
    proc = subprocess.Popen(
        cmd,
        cwd=PROJECT_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        errors="replace",
    )
    captured = []
    for line in proc.stdout:
        sys.stdout.write(line)
        captured.append(line)
    proc.wait()
    if proc.returncode != 0:
        doctor.diagnose_build_output("".join(captured))
    return proc.returncode == 0


def main():
    parser = argparse.ArgumentParser(
        description="Evitania Automated Update Runner - Validates signatures, extracts offsets, and rebuilds binaries on game updates."
    )
    parser.add_argument(
        "--check", action="store_true", help="Only check for game updates and exit"
    )
    parser.add_argument(
        "-f",
        "--force",
        action="store_true",
        help="Force update and rebuild even if build ID has not changed",
    )
    parser.add_argument(
        "--no-build",
        action="store_true",
        help="Run signature and offset scanning without compiling the DLL",
    )
    parser.add_argument(
        "--build-type",
        default="release",
        choices=["release", "debug", "clean"],
        help="Build profile (default: release)",
    )
    parser.add_argument(
        "--report",
        action="store_true",
        help="Save a JSON summary report to update_report.json",
    )
    parser.add_argument(
        "--dll",
        type=str,
        default=None,
        help="Path to custom GameAssembly.dll (e.g. dumped from RAM)",
    )
    parser.add_argument(
        "--from-mem",
        action="store_true",
        help="Dump unpacked GameAssembly.dll from live game process before updating",
    )

    parser.add_argument(
        "--doctor",
        action="store_true",
        help="Run read-only health diagnostics (env, game files, dump.cs, signatures, C++ refs) and exit",
    )
    parser.add_argument(
        "--no-heal",
        action="store_true",
        help="Disable automatic self-healing when signatures break",
    )
    parser.add_argument(
        "--allow-degraded",
        action="store_true",
        help="If healing cannot fix everything, still build: broken hooks are disabled at runtime and their offsets keep last-known-good values",
    )
    parser.add_argument(
        "--set-version",
        type=str,
        default=None,
        help="Set the target game version in config.json (e.g. 0.324.0)",
    )
    parser.add_argument(
        "--bump",
        action="store_true",
        help="Fallback patch bump if real game version cannot be auto-detected from Unity PlayerSettings",
    )

    args = parser.parse_args()

    if args.doctor:
        sys.exit(
            doctor.run_doctor(
                args.dll,
                args.report and os.path.join(PROJECT_ROOT, "doctor_report.json"),
            )
        )

    print_banner("EVITANIA AUTOMATED UPDATE RUNNER")

    # 1. Locate Game Assembly & Steam Manifest
    if args.from_mem:
        print("[*] '--from-mem' specified. Invoking live process memory dumper...")
        dumper_script = os.path.join(SCRIPT_DIR, "dump_memory.py")
        ret = subprocess.run([sys.executable, dumper_script], cwd=PROJECT_ROOT)
        if ret.returncode != 0:
            print("[-] Failed to dump module from live process RAM.")
            sys.exit(1)
        dll_path = os.path.join(
            PROJECT_ROOT,
            "resources",
            "dumped",
            "GameAssembly_memory.dll",
        )
    elif args.dll:
        dll_path = os.path.abspath(args.dll)
    else:
        dll_path = find_game_assembly()

    if not os.path.exists(dll_path):
        print(f"[-] Error: GameAssembly.dll not found at: {dll_path}")
        sys.exit(1)

    print(f"[+] Located GameAssembly.dll: {dll_path}")
    current_sha256 = get_file_sha256(dll_path)
    print(f"[+] Assembly SHA-256: {current_sha256[:16]}...")

    manifest_path = find_steam_manifest(dll_path)
    manifest_info = read_manifest_info(manifest_path)
    current_build_id = manifest_info["buildid"]
    print(f"[+] Steam Build ID:   {current_build_id} (AppID: {APP_ID})")

    detected_version = detect_game_version(dll_path)
    if detected_version:
        print(f"[+] Game Version:     {detected_version} (from Unity PlayerSettings)")

    # 2. Check config.json Metadata
    if not os.path.exists(CONFIG_PATH):
        print(f"[-] Error: config.json not found at {CONFIG_PATH}")
        sys.exit(1)

    with open(CONFIG_PATH, "r", encoding="utf-8") as f:
        config = json.load(f)

    recorded_build_id = config.get("steam_build_id")
    recorded_sha256 = config.get("assembly_sha256")
    recorded_version = config.get("version", "unknown")
    game_version = recorded_version

    if args.set_version:
        game_version = args.set_version.lstrip("v")
        config["version"] = game_version
        print(f"[+] Setting target game version -> {game_version}")
        if detected_version and game_version != detected_version:
            print(
                f"[!] Warning: --set-version ({game_version}) differs from detected game version ({detected_version})."
            )
    elif detected_version:
        game_version = detected_version
        config["version"] = game_version
        if args.bump:
            print(
                f"[!] Ignoring blind --bump; using authoritative Unity PlayerSettings version -> {game_version}"
            )
        elif game_version != recorded_version:
            print(
                f"[+] Synced target game version: {recorded_version} -> {game_version}"
            )
    elif args.bump:
        parts = game_version.split(".")
        if parts and parts[-1].isdigit():
            parts[-1] = str(int(parts[-1]) + 1)
            game_version = ".".join(parts)
            config["version"] = game_version
            print(
                f"[+] Bumped target game version -> {game_version} (Unity PlayerSettings unavailable)"
            )

    is_updated = (
        recorded_build_id != current_build_id
        or recorded_sha256 != current_sha256
        or recorded_version != game_version
    )

    if args.check:
        if is_updated:
            print(
                f"\n[!] UPDATE AVAILABLE: Steam Build {current_build_id} (recorded {recorded_build_id}), Version {game_version} (recorded {recorded_version})"
            )
            sys.exit(1)
        else:
            print(
                f"\n[+] Game is UP TO DATE (Build {current_build_id}, v{game_version})."
            )
            sys.exit(0)

    if not is_updated and not args.force:
        print(f"\n[*] Game binary is already up to date:")
        print(f"    - Game Version:    v{game_version} (Matches recorded)")
        print(f"    - Steam Build ID:  {current_build_id} (Matches recorded)")
        print(f"    - Assembly SHA256: {current_sha256[:16]}... (Matches recorded)")
        print(
            "\n[*] No update necessary. Use '--force' or '-f' to re-scan signatures and rebuild anyway."
        )
        sys.exit(0)

    if is_updated:
        print(f"\n[!] Game update detected!")
        if recorded_version != game_version:
            print(f"    - Game Version:   {recorded_version} -> {game_version}")
        print(f"    - Steam Build:    {recorded_build_id} -> {current_build_id}")
        print(
            f"    - Assembly Hash:  {str(recorded_sha256)[:16]}... -> {current_sha256[:16]}..."
        )
    else:
        print(f"\n[*] Running forced update scan and rebuild...")

    # 3. Execute Signature Scanning & Semantic Offset Extraction
    doctor.snapshot()
    heal_log = []
    start_time = time.time()
    results = generate_signatures_and_offsets(
        dll_path=dll_path, sig_db_path=CONFIG_PATH, verbose=True
    )
    elapsed = time.time() - start_time

    # 4. Validation Gate & Intelligent In-Memory Fallback
    if not results["success"] and not args.from_mem and not args.dll:
        broken_count = len(results.get("broken_signatures", []))
        if broken_count > 0:
            print(
                f"\n[!] Detected {broken_count} signature scan failures on disk binary."
            )
            pids = (
                find_pids_linux("Evitania")
                if platform.system() == "Linux"
                else find_pids_windows("Evitania")
            )
            if pids:
                pid, name = pids[0]
                print(f"[!] Active game process detected: PID {pid} ({name})")
                print(
                    "[*] Initiating intelligent fallback: Dumping decrypted GameAssembly.dll from live process RAM..."
                )
                dumper_script = os.path.join(SCRIPT_DIR, "dump_memory.py")
                ret = subprocess.run(
                    [sys.executable, dumper_script, "--pid", str(pid)], cwd=PROJECT_ROOT
                )
                if ret.returncode == 0:
                    mem_dll = os.path.join(
                        PROJECT_ROOT, "resources", "dumped", "GameAssembly_memory.dll"
                    )
                    if os.path.exists(mem_dll):
                        print(
                            "[+] RAM dump completed. Re-running signature & semantic offset extraction against memory image..."
                        )
                        results = generate_signatures_and_offsets(
                            dll_path=mem_dll, sig_db_path=CONFIG_PATH, verbose=True
                        )
                        if results["success"]:
                            print(
                                "\n[+] Intelligent Fallback Succeeded: 100% signatures & offsets resolved from live RAM!"
                            )
                            dll_path = mem_dll
                            current_sha256 = get_file_sha256(mem_dll)
            else:
                print(
                    "[*] Game is not currently running. If the disk binary is encrypted/packed, launch Evitania and run `scripts/update --from-mem`."
                )

    # 4b. Self-heal: regenerate dump.cs / signature DB and re-scan.
    if not results["success"] and not args.no_heal:
        print_banner("SELF-HEAL")
        healed, dll_path, heal_log = doctor.heal(
            dll_path,
            lambda d: generate_signatures_and_offsets(
                dll_path=d, sig_db_path=CONFIG_PATH, verbose=False
            ),
        )
        if healed is not None:
            results = healed
            current_sha256 = get_file_sha256(dll_path)
        with open(CONFIG_PATH, "r", encoding="utf-8") as f:
            config = json.load(f)  # build_sig_db rewrote signatures
        print(
            "[+] Self-heal SUCCEEDED."
            if results["success"]
            else "[-] Self-heal incomplete."
        )

    # 4c. Degraded mode: keep last-known-good entries so the DLL still compiles.
    if (
        not results["success"]
        and args.allow_degraded
        and not results["ambiguous_signatures"]
    ):
        print_banner("DEGRADED BUILD")
        # Restore previous headers first so last-known-good values can be carried over.
        for p in (doctor.SIG_HPP, doctor.OFF_HPP):
            snap = os.path.join(doctor.SNAPSHOT_DIR, os.path.basename(p))
            if os.path.exists(snap):
                shutil.copy2(snap, p)
        results = generate_signatures_and_offsets(
            dll_path=dll_path, sig_db_path=CONFIG_PATH, verbose=False, degraded=True
        )
        unresolved = [o for o in results["failed_offsets"]]
        if not unresolved:
            print(
                f"[!] {len(results['degraded_signatures'])} hook(s) disabled: {', '.join(results['degraded_signatures'])}"
            )
            if results["stale_offsets"]:
                print(
                    f"[!] Stale offsets (verify!): {', '.join(results['stale_offsets'])}"
                )
            results["success"] = True
            results["degraded"] = True

    if not results["success"]:
        if not args.no_heal:
            doctor.rollback()
        print_banner("UPDATE FAILED - SIGNATURE / OFFSET MISMATCH")
        if results["broken_signatures"]:
            print(f"[-] Broken Signatures ({len(results['broken_signatures'])}):")
            for sig in results["broken_signatures"]:
                print(f"    - {sig}")
        if results["ambiguous_signatures"]:
            print(f"[-] Ambiguous Signatures ({len(results['ambiguous_signatures'])}):")
            for sig, count in results["ambiguous_signatures"]:
                print(f"    - {sig} (matched {count} locations)")
        if results["failed_offsets"]:
            print(f"[-] Failed Offsets ({len(results['failed_offsets'])}):")
            for off in results["failed_offsets"]:
                print(f"    - {off}")

        print("\n[*] Diagnosis:")
        doctor.check_scan_and_dump(doctor.Report(), dll_path, config)
        print("\n[*] Recommended Actions:")
        print(
            "    1. If disk binary is packed or encrypted, launch the game and run: `scripts/update --from-mem`."
        )
        print(
            "    2. Run `python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> --disasm` to inspect changes."
        )
        print(
            "    3. If the game is obfuscated, run the Deobfuscator Bridge (`python3 resources/Tools/deobfuscator.py --all`)."
        )
        print(
            "    4. Build anyway with broken hooks disabled: `scripts/update --force --allow-degraded`."
        )
        sys.exit(1)

    # 5. Update config.json metadata
    ordered_config = {
        "project_name": config.get("project_name", "Evitania"),
        "version": game_version,
        "steam_build_id": current_build_id,
        "assembly_sha256": current_sha256,
        "signatures": config.get("signatures", {}),
    }

    with open(CONFIG_PATH, "w", encoding="utf-8") as f:
        json.dump(ordered_config, f, indent="\t")
    print(
        f"\n[+] Updated config.json with Steam Build ID {current_build_id} and Assembly SHA-256."
    )

    # 6. Build the Project
    build_success = True
    if not args.no_build:
        build_success = run_build(args.build_type)
        if not build_success:
            print_banner("BUILD FAILED")
            print(
                "[-] Compilation failed. Check the build logs / doctor diagnosis above."
            )
            print(
                "[*] config.json and headers are kept (signatures are valid); fix the C++ and run `scripts/build release`."
            )
            sys.exit(1)

    # 7. Summary Report
    if heal_log:
        print(f"\n[*] Self-heal actions: {'; '.join(heal_log)}")
    print_banner("UPDATE & EXTRACTION SUMMARY")
    print(f"  Game Version:        Evitania v{game_version}")
    print(f"  Steam Build ID:      {current_build_id}")
    print(f"  Assembly SHA-256:    {current_sha256[:20]}...")
    print(
        f"  Signatures Scanned:  {results['matched_signatures']} / {results['total_signatures']}{' (DEGRADED)' if results.get('degraded') else ' (100% Validated)'}"
    )
    print(
        f"  Offsets Extracted:   {results['total_offsets']} total ({results['semantic_matches']} semantic, {results['hardcoded_offsets']} hardcoded)"
    )
    if results["semantic_shifts"]:
        print(
            f"  Semantic Shifts:     {len(results['semantic_shifts'])} auto-resolved:"
        )
        for shift in results["semantic_shifts"]:
            print(f"    - {shift}")
    else:
        print(f"  Semantic Shifts:     0 (Instructions remained in place)")
    print(f"  Scan Duration:       {elapsed:.2f}s")
    if not args.no_build:
        print(
            f"  Binary Generated:    build/{args.build_type}/Evitania v{game_version}.dll"
        )
        status = (
            "DEGRADED - some hooks disabled"
            if results.get("degraded")
            else "READY TO RUN (scripts/run)"
        )
        print(f"  Status:              {status}")
    else:
        print(f"  Status:              HEADERS GENERATED (--no-build specified)")
    print("=" * 65)

    if args.report:
        report_data = {
            "version": game_version,
            "steam_build_id": current_build_id,
            "assembly_sha256": current_sha256,
            "scan_duration_seconds": round(elapsed, 2),
            "stats": results,
        }
        report_path = os.path.join(PROJECT_ROOT, "update_report.json")
        with open(report_path, "w") as f:
            json.dump(report_data, f, indent=2)
        print(f"[+] Saved summary report -> {report_path}")

    sys.exit(0)


if __name__ == "__main__":
    main()
