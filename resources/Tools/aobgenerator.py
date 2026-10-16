import os
import re
import json
import pefile
import platform
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM


def find_game_assembly():
    steam_paths = []
    if platform.system() == "Windows":
        try:
            import winreg

            with winreg.OpenKey(
                winreg.HKEY_CURRENT_USER, r"Software\Valve\Steam"
            ) as key:
                steam_path = winreg.QueryValueEx(key, "SteamPath")[0]
                steam_paths.append(steam_path)
        except Exception:
            pass
        for p in [r"C:\Program Files (x86)\Steam", r"C:\Program Files\Steam"]:
            if os.path.exists(p) and p not in steam_paths:
                steam_paths.append(p)
    elif platform.system() == "Linux":
        for p in [
            os.path.expanduser("~/.steam/steam"),
            os.path.expanduser("~/.local/share/Steam"),
        ]:
            if os.path.exists(p):
                steam_paths.append(p)
    elif platform.system() == "Darwin":
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
                with open(vdf_path, "r", encoding="utf-8") as f:
                    content = f.read()
                    paths = re.findall(r'"path"\s+"([^"]+)"', content)
                    for p in paths:
                        clean_path = p.replace("\\\\", "\\")
                        if clean_path not in library_folders:
                            library_folders.append(clean_path)
            except Exception:
                pass

    app_id = "4119420"
    for lib in library_folders:
        manifest_path = os.path.join(lib, "steamapps", f"appmanifest_{app_id}.acf")
        if os.path.exists(manifest_path):
            try:
                with open(manifest_path, "r", encoding="utf-8") as f:
                    content = f.read()
                    match = re.search(
                        r'"installdir"\s+"([^"]+)"', content, re.IGNORECASE
                    )
                    if match:
                        install_dir = match.group(1)
                        dll_path = os.path.join(
                            lib, "steamapps", "common", install_dir, "GameAssembly.dll"
                        )
                        if os.path.exists(dll_path):
                            print(
                                f"[+] Found GameAssembly.dll via AppID {app_id} at: {dll_path}"
                            )
                            return dll_path
            except Exception:
                pass

    fallback_path = os.path.join(
        os.path.dirname(__file__), "../dumped/GameAssembly.dll"
    )
    print(
        f"[-] Could not find game in Steam libraries. Falling back to: {fallback_path}"
    )
    return fallback_path


DLL_PATH = find_game_assembly()
SIG_DB_PATH = os.path.join(
    os.path.dirname(__file__), "../../config.json"
)  # Root Folder of Project


def parse_pattern_to_regex(pattern):
    regex_str = b""
    for b in pattern.split():
        if b == "?":
            regex_str += b"."
        else:
            regex_str += re.escape(bytes([int(b, 16)]))
    return regex_str


def compile_pattern_to_regex(pattern, relaxed_registers=False):
    # Escape regex specials except ? and *
    p = pattern.replace("?", "__DISP__").replace("*", "__WILDCARD__")
    if relaxed_registers:
        reg_pattern = r"\b(?:r[a-d]x|r[s|b]p|r[s|d]i|r[8-9][a-z0-9]?|r1[0-5][a-z0-9]?|e[a-d]x|e[s|b]p|e[s|d]i|xmm\d+|ymm\d+)\b"
        p = re.sub(reg_pattern, "__REG__", p, flags=re.IGNORECASE)

    p = re.escape(p)
    p = p.replace("__DISP__", r"(?:0x[0-9a-fA-F]+|\d+)")
    p = p.replace("__WILDCARD__", r"[a-zA-Z0-9_]+")
    if relaxed_registers:
        p = p.replace("__REG__", r"(?:r[a-z0-9]+|e[a-z0-9]+|xmm\d+|ymm\d+)")
    p = re.sub(r"\\ ", r"\\s+", p)
    p = re.sub(r"\\\[", r"\\[\\s*", p)
    p = re.sub(r"\\\]", r"\\s*\\]", p)
    p = re.sub(r"\\\+", r"\\s*\\+\\s*", p)
    p = re.sub(r"\\,", r"\\s*,\\s*", p)
    return re.compile(f"^{p}$", re.IGNORECASE)


def extract_offset_semantic(instructions, rule):
    """
    Extracts field offset using semantic assembly pattern matching with fallback to instr_idx/op_idx.
    Supports two passes:
      Pass 1: Strict register matching (exact registers as written in config.json)
      Pass 2: Relaxed register matching (immune to compiler register reassignment)
    Returns: (offset_val, status_str)
    """
    if "hardcoded" in rule:
        return rule["hardcoded"], "hardcoded fallback"

    pattern = rule.get("pattern")
    fallback_idx = rule.get("instr_idx")
    fallback_op_idx = rule.get("op_idx", 0)

    if pattern:
        is_mem_disp = bool(re.search(r"\[[^\]]*\?[^\]]*\]", pattern))

        # Two-pass match: Pass 1 (Strict), Pass 2 (Relaxed Registers)
        for pass_num, relaxed in [(1, False), (2, True)]:
            pat_rgx = compile_pattern_to_regex(pattern, relaxed_registers=relaxed)
            candidates = []

            for i, ins in enumerate(instructions):
                text = f"{ins.mnemonic} {ins.op_str}"
                if pat_rgx.match(text):
                    disp = None
                    if "op_idx" in rule and rule["op_idx"] < len(ins.operands):
                        target_op = ins.operands[rule["op_idx"]]
                        if target_op.type == CS_OP_MEM:
                            disp = target_op.mem.disp
                        elif target_op.type == CS_OP_IMM:
                            disp = target_op.imm

                    if disp is None:
                        if is_mem_disp:
                            for op in ins.operands:
                                if op.type == CS_OP_MEM:
                                    disp = op.mem.disp
                                    break
                        else:
                            for op in ins.operands:
                                if op.type == CS_OP_IMM:
                                    disp = op.imm
                                    break

                    if disp is not None:
                        candidates.append((i, text, disp))

            if candidates:
                if "match_idx" in rule and rule["match_idx"] < len(candidates):
                    best = candidates[rule["match_idx"]]
                elif fallback_idx is not None:
                    best = min(candidates, key=lambda c: abs(c[0] - fallback_idx))
                else:
                    best = candidates[0]

                idx, matched_text, val = best
                mode_str = "strict" if not relaxed else "relaxed-reg"
                if fallback_idx is not None and idx != fallback_idx:
                    status = (
                        f"semantic shift [{mode_str}] (idx {fallback_idx} -> {idx})"
                    )
                else:
                    status = f"semantic match [{mode_str}] at idx {idx}"
                return val, status

    # Fallback to direct instruction index if pattern not provided or not matched
    if fallback_idx is not None and fallback_idx < len(instructions):
        ins = instructions[fallback_idx]
        if fallback_op_idx < len(ins.operands):
            op = ins.operands[fallback_op_idx]
            if op.type == CS_OP_MEM:
                return op.mem.disp, f"fallback instr_idx {fallback_idx}"
            elif op.type == CS_OP_IMM:
                return op.imm, f"fallback instr_idx {fallback_idx}"

    return None, "failed"


def generate_signatures_and_offsets(dll_path=None, sig_db_path=None, verbose=True):
    target_dll = dll_path or DLL_PATH
    target_config = sig_db_path or SIG_DB_PATH

    results = {
        "success": False,
        "total_signatures": 0,
        "matched_signatures": 0,
        "broken_signatures": [],
        "ambiguous_signatures": [],
        "total_offsets": 0,
        "semantic_matches": 0,
        "semantic_shifts": [],
        "hardcoded_offsets": 0,
        "failed_offsets": [],
        "sig_path": None,
        "off_path": None,
    }

    if not os.path.exists(target_dll):
        if verbose:
            print(f"[-] Error: GameAssembly.dll not found at {target_dll}")
        return results

    if not os.path.exists(target_config):
        if verbose:
            print(f"[-] Error: config.json not found at {target_config}")
        return results

    if verbose:
        print("[*] Loading config.json...")
    with open(target_config, "r") as f:
        config = json.load(f)
        sig_db = config.get("signatures", {})

    if verbose:
        print("[*] Loading GameAssembly.dll...")
    pe = pefile.PE(target_dll)
    pe_data = pe.get_memory_mapped_image()

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    signatures_content = [
        "#pragma once",
        "",
        "namespace Signatures",
        "{",
    ]

    il2cpp_offsets = {}
    game_offsets = {}

    if verbose:
        print("\n--- Scanning & Extracting ---")
    for method_key, data in sig_db.items():
        pattern = data["signature"]
        extract_rules = data.get("extract", {})

        if method_key.startswith("Il2Cpp_"):
            struct_name = method_key.replace("Il2Cpp_", "")
            if struct_name not in il2cpp_offsets:
                il2cpp_offsets[struct_name] = {}
            for field_name, rule in extract_rules.items():
                if "hardcoded" in rule:
                    il2cpp_offsets[struct_name][field_name] = rule["hardcoded"]
                    results["total_offsets"] += 1
                    results["hardcoded_offsets"] += 1
                    if verbose:
                        print(
                            f"    -> Extracted Il2Cpp::{struct_name}::{field_name}: 0x{rule['hardcoded']:X} (hardcoded fallback)"
                        )
            continue

        if not pattern:
            for field_name, rule in extract_rules.items():
                if "hardcoded" in rule:
                    game_offsets[field_name] = rule["hardcoded"]
                    results["total_offsets"] += 1
                    results["hardcoded_offsets"] += 1
                    if verbose:
                        print(
                            f"    -> Extracted {field_name}: 0x{rule['hardcoded']:X} (hardcoded fallback)"
                        )
            continue

        results["total_signatures"] += 1
        regex_str = parse_pattern_to_regex(pattern)
        regex = re.compile(regex_str, re.DOTALL)

        matches = list(regex.finditer(pe_data))
        if len(matches) == 1:
            results["matched_signatures"] += 1
            rva = matches[0].start()
            if verbose:
                print(f"[+] Found {method_key} at RVA: 0x{rva:X}")

            signatures_content.append(f"  // 0x{rva:X}")
            signatures_content.append(
                f'  constexpr const char* {method_key} = "{pattern}";'
            )
            signatures_content.append("")

            code = pe_data[rva : rva + 1200]
            instructions = list(md.disasm(code, rva))

            for field_name, rule in extract_rules.items():
                results["total_offsets"] += 1
                val, status = extract_offset_semantic(instructions, rule)
                if val is not None:
                    game_offsets[field_name] = val
                    if "semantic shift" in status:
                        results["semantic_shifts"].append(
                            f"{method_key}::{field_name} ({status})"
                        )
                    elif "semantic match" in status:
                        results["semantic_matches"] += 1
                    elif "hardcoded" in status:
                        results["hardcoded_offsets"] += 1

                    if verbose:
                        print(f"    -> Extracted {field_name}: 0x{val:X} ({status})")
                else:
                    results["failed_offsets"].append(f"{method_key}::{field_name}")
                    if verbose:
                        print(f"    [-] Failed to extract {field_name}")
        elif len(matches) == 0:
            results["broken_signatures"].append(method_key)
            if verbose:
                print(f"[-] Could not find match for {method_key} (Broken)")
        else:
            results["ambiguous_signatures"].append((method_key, len(matches)))
            if verbose:
                print(
                    f"[-] Could not find unique match for {method_key} (Matches: {len(matches)})"
                )

    signatures_content.append("}  // namespace Signatures")

    offsets_content = [
        "#pragma once",
        "",
        "namespace Offsets",
        "{",
        "  // IL2CPP Internal Structures",
        "  namespace Il2Cpp",
        "  {",
    ]

    for struct_name, fields in il2cpp_offsets.items():
        offsets_content.append(f"    namespace {struct_name}")
        offsets_content.append("    {")
        for field_name, val in fields.items():
            offsets_content.append(f"      constexpr int {field_name:<11} = 0x{val:X};")
        offsets_content.append(f"    }}  // namespace {struct_name}")
        offsets_content.append("")

    offsets_content.extend(
        [
            "  }  // namespace Il2Cpp",
            "",
            "  // Backward-compatible aliases for IL2CPP generic offsets",
            "  constexpr int entries = Il2Cpp::Dictionary::entries;",
            "  constexpr int count   = Il2Cpp::Dictionary::count;",
            "",
            "  // Game Logic Field Offsets",
        ]
    )

    for field_name, val in game_offsets.items():
        offsets_content.append(f"  constexpr int {field_name:<16} = 0x{val:X};")
    offsets_content.append("}  // namespace Offsets")

    sig_path = os.path.join(
        os.path.dirname(__file__), "../../src/Modules/Hooks/Signatures.hpp"
    )
    off_path = os.path.join(
        os.path.dirname(__file__), "../../src/Modules/Hooks/Offsets.hpp"
    )

    with open(sig_path, "w") as f:
        f.write("\n".join(signatures_content))

    with open(off_path, "w") as f:
        f.write("\n".join(offsets_content))

    results["sig_path"] = sig_path
    results["off_path"] = off_path
    results["success"] = (
        len(results["broken_signatures"]) == 0
        and len(results["ambiguous_signatures"]) == 0
        and len(results["failed_offsets"]) == 0
    )

    if verbose:
        print(f"\n[+] Generated {sig_path}")
        print(f"[+] Generated {off_path}")

    return results


def main():
    generate_signatures_and_offsets()


if __name__ == "__main__":
    main()
