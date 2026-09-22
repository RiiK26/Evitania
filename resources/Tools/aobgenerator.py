import os
import re
import json
import pefile
import platform
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM

def find_game_assembly():
    steam_paths = []
    if platform.system() == 'Windows':
        try:
            import winreg
            with winreg.OpenKey(winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam") as key:
                steam_path = winreg.QueryValueEx(key, "SteamPath")[0]
                steam_paths.append(steam_path)
        except Exception:
            pass
        for p in [r"C:\Program Files (x86)\Steam", r"C:\Program Files\Steam"]:
            if os.path.exists(p) and p not in steam_paths:
                steam_paths.append(p)
    elif platform.system() == 'Linux':
        for p in [os.path.expanduser("~/.steam/steam"), os.path.expanduser("~/.local/share/Steam")]:
            if os.path.exists(p):
                steam_paths.append(p)
    elif platform.system() == 'Darwin':
        p = os.path.expanduser("~/Library/Application Support/Steam")
        if os.path.exists(p):
            steam_paths.append(p)

    library_folders = []
    for sp in steam_paths:
        if sp not in library_folders:
            library_folders.append(sp)
        vdf_path = os.path.join(sp, "steamapps", "libraryfolders.vdf")
        if os.path.exists(vdf_path):
            try:
                with open(vdf_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    paths = re.findall(r'"path"\s+"([^"]+)"', content)
                    for p in paths:
                        clean_path = p.replace('\\\\', '\\')
                        if clean_path not in library_folders:
                            library_folders.append(clean_path)
            except Exception:
                pass

    app_id = "4119420"
    for lib in library_folders:
        manifest_path = os.path.join(lib, "steamapps", f"appmanifest_{app_id}.acf")
        if os.path.exists(manifest_path):
            try:
                with open(manifest_path, 'r', encoding='utf-8') as f:
                    content = f.read()
                    match = re.search(r'"installdir"\s+"([^"]+)"', content, re.IGNORECASE)
                    if match:
                        install_dir = match.group(1)
                        dll_path = os.path.join(lib, "steamapps", "common", install_dir, "GameAssembly.dll")
                        if os.path.exists(dll_path):
                            print(f"[+] Found GameAssembly.dll via AppID {app_id} at: {dll_path}")
                            return dll_path
            except Exception:
                pass

    fallback_path = os.path.join(os.path.dirname(__file__), "../dumped/GameAssembly.dll")
    print(f"[-] Could not find game in Steam libraries. Falling back to: {fallback_path}")
    return fallback_path

DLL_PATH = find_game_assembly()
SIG_DB_PATH = os.path.join(os.path.dirname(__file__), "../../config.json") # Root Folder of Project

def parse_pattern_to_regex(pattern):
    regex_str = b""
    for b in pattern.split():
        if b == '?':
            regex_str += b"."
        else:
            regex_str += re.escape(bytes([int(b, 16)]))
    return regex_str

def extract_offset_from_instruction(pe_data, rva, md, instr_idx, op_idx):
    code = pe_data[rva:rva+800]
    for i, instr in enumerate(md.disasm(code, rva)):
        if i == instr_idx:
            if op_idx < len(instr.operands):
                op = instr.operands[op_idx]
                if op.type == CS_OP_MEM:
                    return op.mem.disp
                elif op.type == CS_OP_IMM:
                    return op.imm
    return None

def main():
    if not os.path.exists(DLL_PATH):
        print(f"[-] Error: GameAssembly.dll not found at {DLL_PATH}")
        return

    if not os.path.exists(SIG_DB_PATH):
        print(f"[-] Error: config.json not found at {SIG_DB_PATH}")
        return

    print("[*] Loading config.json...")
    with open(SIG_DB_PATH, "r") as f:
        config = json.load(f)
        sig_db = config.get("signatures", {})

    print("[*] Loading GameAssembly.dll...")
    pe = pefile.PE(DLL_PATH)
    pe_data = pe.get_memory_mapped_image()

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    signatures_content = [
        "#pragma once",
        "",
        "namespace Signatures",
        "{",
    ]

    offsets_content = [
        "#pragma once",
        "",
        "namespace Offsets",
        "{",
    ]

    extracted_offsets = {}

    print("\n--- Scanning & Extracting ---")
    for method_key, data in sig_db.items():
        pattern = data["signature"]
        extract_rules = data.get("extract", {})

        regex_str = parse_pattern_to_regex(pattern)
        regex = re.compile(regex_str, re.DOTALL)

        matches = list(regex.finditer(pe_data))
        if len(matches) == 1:
            rva = matches[0].start()
            print(f"[+] Found {method_key} at RVA: 0x{rva:X}")

            signatures_content.append(f"  // 0x{rva:X}")
            signatures_content.append(f"  constexpr const char* {method_key} = \"{pattern}\";")
            signatures_content.append("")

            for field_name, rule in extract_rules.items():
                if "hardcoded" in rule:
                    extracted_offsets[field_name] = rule["hardcoded"]
                    print(f"    -> Extracted {field_name}: 0x{rule['hardcoded']:X} (hardcoded fallback)")
                else:
                    instr_idx = rule["instr_idx"]
                    op_idx = rule["op_idx"]
                    val = extract_offset_from_instruction(pe_data, rva, md, instr_idx, op_idx)
                    if val is not None:
                        extracted_offsets[field_name] = val
                        print(f"    -> Extracted {field_name}: 0x{val:X}")
                    else:
                        print(f"    [-] Failed to extract {field_name}")
        else:
            print(f"[-] Could not find unique match for {method_key} (Matches: {len(matches)})")

    signatures_content.append("}  // namespace Signatures")

    for field_name, val in extracted_offsets.items():
        offsets_content.append(f"  constexpr int {field_name} = 0x{val:X};")
    offsets_content.append("}  // namespace Offsets")

    sig_path = os.path.join(os.path.dirname(__file__), "../../src/Modules/Hooks/Signatures.hpp")
    off_path = os.path.join(os.path.dirname(__file__), "../../src/Modules/Hooks/Offsets.hpp")

    with open(sig_path, "w") as f:
        f.write("\n".join(signatures_content))

    with open(off_path, "w") as f:
        f.write("\n".join(offsets_content))

    print(f"\n[+] Generated {sig_path}")
    print(f"[+] Generated {off_path}")

if __name__ == "__main__":
    main()
