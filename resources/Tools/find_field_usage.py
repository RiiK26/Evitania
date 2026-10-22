#!/usr/bin/env python3

import argparse
import json
import os
import platform
import re
import sys
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM

DUMP_PATH = os.path.join(os.path.dirname(__file__), "../dumped/dump.cs")
DUMP_DIR = os.path.dirname(DUMP_PATH)


def ensure_dump_exists():
    if not os.path.exists(DUMP_DIR):
        os.makedirs(DUMP_DIR, exist_ok=True)
        print(f"[-] Created directory: {DUMP_DIR}")
        print(
            "[-] Please place dump.cs from Il2CppDumper in this directory before running."
        )
        sys.exit(1)
    elif not os.path.exists(DUMP_PATH):
        print(f"[-] dump.cs not found at {DUMP_PATH}")
        print(
            "[-] Please place dump.cs from Il2CppDumper in this directory before running."
        )
        sys.exit(1)


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


def get_offsets():
    ensure_dump_exists()
    class_data = {}
    current_class = None

    with open(DUMP_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    class_regex = re.compile(
        r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract)\s+)*(?:class|struct|interface|enum)\s+([<>\w.]+)"
    )
    rva_regex = re.compile(r"// RVA: 0x([0-9A-Fa-f]+)")
    method_regex = re.compile(
        r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract|override|virtual)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+|\.ctor)\s*\("
    )
    # Support compiler-generated backing fields with angle brackets e.g. <Amount>k__BackingField
    field_regex = re.compile(
        r"^\s*(?:(?:public|private|protected|internal|static|readonly|const|volatile)\s+)*.*?\s+([<>\w]+|[a-zA-Z0-9_]+);\s*//\s*0x([0-9A-Fa-f]+)"
    )

    for i, line in enumerate(lines):
        c_match = class_regex.search(line)
        if c_match:
            current_class = c_match.group(1)
            if current_class not in class_data:
                class_data[current_class] = {"methods": {}, "fields": {}}
            continue

        if current_class:
            f_match = field_regex.search(line)
            if f_match:
                class_data[current_class]["fields"][f_match.group(1)] = int(
                    f_match.group(2), 16
                )
                continue

            rva_match = rva_regex.search(line)
            if rva_match:
                k = 1
                while i + k < len(lines):
                    nl = lines[i + k].strip()
                    if not nl:
                        k += 1
                        continue
                    if nl.startswith("// RVA:") or class_regex.search(lines[i + k]):
                        break
                    if nl.startswith("//"):
                        k += 1
                        continue
                    m_match = method_regex.search(lines[i + k])
                    if m_match:
                        method_name = m_match.group(1)
                        rva_val = int(rva_match.group(1), 16)
                        if method_name not in class_data[current_class]["methods"]:
                            class_data[current_class]["methods"][method_name] = []
                        class_data[current_class]["methods"][method_name].append(
                            rva_val
                        )
                    break

    # If deobfuscation map exists, register canonical aliases
    deobf_map_path = os.path.join(DUMP_DIR, "deobfuscation_map.json")
    if os.path.exists(deobf_map_path):
        try:
            with open(deobf_map_path, "r", encoding="utf-8") as f:
                deobf = json.load(f)
            class_map = deobf.get("classes", {})
            for raw_cls, canon_cls in class_map.items():
                if raw_cls in class_data and canon_cls not in class_data:
                    class_data[canon_cls] = class_data[raw_cls]
            for raw_field_full, canon_field_full in deobf.get("fields", {}).items():
                if "::" in raw_field_full and "::" in canon_field_full:
                    raw_cls, raw_f = raw_field_full.split("::", 1)
                    canon_cls, canon_f = canon_field_full.split("::", 1)
                    if canon_cls in class_data and raw_cls in class_data:
                        if raw_f in class_data[raw_cls]["fields"]:
                            class_data[canon_cls]["fields"][canon_f] = class_data[
                                raw_cls
                            ]["fields"][raw_f]
        except Exception:
            pass

    return class_data


def find_usages(pe_data, md, rva, target_offset, max_bytes=800):
    """
    Scans disassembly for occurrences of target_offset in memory displacement or immediate operands.
    Returns list of dicts: {'instr_idx', 'op_idx', 'op_type', 'mnemonic', 'op_str', 'addr', 'instr_text'}
    """
    code = pe_data[rva : rva + max_bytes]
    matches = []
    for i, instr in enumerate(md.disasm(code, rva)):
        for op_idx, op in enumerate(instr.operands):
            if op.type == CS_OP_MEM and op.mem.disp == target_offset:
                matches.append(
                    {
                        "instr_idx": i,
                        "op_idx": op_idx,
                        "op_type": "MEM",
                        "mnemonic": instr.mnemonic,
                        "op_str": instr.op_str,
                        "addr": instr.address,
                        "instr_text": f"{instr.mnemonic} {instr.op_str}",
                    }
                )
            elif op.type == CS_OP_IMM and op.imm == target_offset:
                matches.append(
                    {
                        "instr_idx": i,
                        "op_idx": op_idx,
                        "op_type": "IMM",
                        "mnemonic": instr.mnemonic,
                        "op_str": instr.op_str,
                        "addr": instr.address,
                        "instr_text": f"{instr.mnemonic} {instr.op_str}",
                    }
                )
    return matches


def disassemble_method(pe_data, md, rva, highlight_offset=None, max_bytes=400):
    """
    Disassembles up to max_bytes from rva and prints numbered instructions.
    """
    code = pe_data[rva : rva + max_bytes]
    print(f"\n--- Disassembly for RVA 0x{rva:X} ---")
    for i, instr in enumerate(md.disasm(code, rva)):
        matched = False
        if highlight_offset is not None:
            for op in instr.operands:
                if (op.type == CS_OP_MEM and op.mem.disp == highlight_offset) or (
                    op.type == CS_OP_IMM and op.imm == highlight_offset
                ):
                    matched = True
                    break

        prefix = "==>" if matched else "   "
        marker = f" [MATCH 0x{highlight_offset:X}]" if matched else ""
        print(
            f"{prefix} [{i:3d}] 0x{instr.address:08X}: {instr.mnemonic:8s} {instr.op_str}{marker}"
        )


DEFAULT_TARGETS = [
    ("AttackReceiver", "Awake", "isPlayer", "AttackReceiver"),
    ("AttackReceiver", "Recieve", "AttackDamage", "Attack"),
    ("EnemyNpcController", "TakeDamage", "alive", "EnemyNpcController"),
    ("EnemyNpcController", "Update", "alive", "EnemyNpcController"),
    ("EngineerService", "TryGetUpgradeCost", "Amount", "EngineerItem"),
    ("HourglassTalentConfig", "get_Cost", "cost", "HourglassTalentConfig"),
    ("ItemDetail", "get_Amount", "<Amount>k__BackingField", "ItemDetail"),
    ("MobilePurchaseService", "InitiatePurchase", "_rewarder", "MobilePurchaseService"),
    ("MovementControl", "Move", "_view", "MovementControl"),
    ("MovementControl", "Move", "networkPlayerSync", "PlayerCharacter"),
    ("MovementControl", "Move", "_isNet", "NetworkPlayerSync"),
    ("SteamPurchaseService", "InitiatePurchase", "_rewarder", "SteamPurchaseService"),
    ("Shop", "UpdateDiscount", "discount", "Shop"),
    (
        "OfflineProgressService",
        "CreateActivitySnapshot",
        "Skill",
        "OfflineActivitySnapshot",
    ),
    (
        "OfflineProgressService",
        "CreateActivitySnapshot",
        "_cardDatabase",
        "OfflineProgressService",
    ),
    (
        "OfflineProgressService",
        "GetCombatCalculator",
        "_combatCalculator",
        "OfflineProgressService",
    ),
    (
        "CombatOfflineRewardsCalculator",
        "SetCurrentSpawner",
        "_currentSpawner",
        "CombatOfflineRewardsCalculator",
    ),
    ("EnemySpawnerService", "get_EnemyCard", "_enemyCards", "EnemySpawnerService"),
    ("EntityCardDatabase", ".ctor", "enemies", "EntityCardDatabase"),
]


def generate_pattern_from_instr_text(instr_text, offset):
    hex_str = f"0x{offset:x}"
    HEX_str = f"0x{offset:X}"
    dec_str = f"{offset}"
    return re.sub(rf"\b(?:{hex_str}|{HEX_str}|{dec_str})\b", "?", instr_text)


def print_rule_snippets(field_name, field_cls, meth_cls, meth_name, offset, match):
    output_name = field_name.replace("<", "").replace(">k__BackingField", "")
    print("\n[+] Suggested build_sig_db.py Target:")
    if output_name == field_name and field_cls == meth_cls:
        print(
            f'    ("{meth_cls}", "{meth_name}", 0, [("{field_name}", "{field_cls}")]),'
        )
    else:
        print(
            f'    ("{meth_cls}", "{meth_name}", 0, [("{output_name}", "{field_name}", "{field_cls}")]),'
        )

    print("\n[+] Suggested config.json Extraction Rule:")
    pattern = generate_pattern_from_instr_text(match["instr_text"], offset)
    rule = {
        output_name: {
            "pattern": pattern,
            "instr_idx": match["instr_idx"],
            "op_idx": match["op_idx"],
        }
    }
    print("    " + json.dumps(rule, indent=8).strip())


def main():
    parser = argparse.ArgumentParser(
        description="Helper tool to find instruction index and operand index of field offsets in method assembly for Evitania signature extractions."
    )
    parser.add_argument(
        "class_name", nargs="?", help="Class name (e.g., Shop, ItemDetail)"
    )
    parser.add_argument(
        "method_name", nargs="?", help="Method name (e.g., UpdateDiscount, get_Amount)"
    )
    parser.add_argument(
        "field_name",
        nargs="?",
        help="Field name (e.g., discount, <Amount>k__BackingField)",
    )
    parser.add_argument(
        "field_class",
        nargs="?",
        help="Class holding the field (defaults to class_name)",
    )

    parser.add_argument(
        "--rva",
        type=lambda x: int(x, 0),
        help="Direct function RVA (hex or dec, e.g., 0x783B90)",
    )
    parser.add_argument(
        "--offset",
        type=lambda x: int(x, 0),
        help="Direct field offset (hex or dec, e.g., 0x58)",
    )
    parser.add_argument(
        "--disasm", action="store_true", help="Print numbered disassembly of the method"
    )
    parser.add_argument(
        "--max-bytes",
        type=int,
        default=800,
        help="Max bytes to disassemble (default 800)",
    )
    parser.add_argument(
        "--list-fields", metavar="CLASS", help="List all fields and offsets for a class"
    )
    parser.add_argument(
        "--list-methods", metavar="CLASS", help="List all methods and RVAs for a class"
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Run audit on all default project extraction targets",
    )

    args = parser.parse_args()

    class_data = get_offsets()

    if args.list_fields:
        cls = args.list_fields
        if cls not in class_data:
            print(f"[-] Class '{cls}' not found in dump.cs")
            sys.exit(1)
        print(f"\n[+] Fields in '{cls}':")
        for fname, off in sorted(class_data[cls]["fields"].items(), key=lambda x: x[1]):
            print(f"    0x{off:02X} ({off:3d}) : {fname}")
        return

    if args.list_methods:
        cls = args.list_methods
        if cls not in class_data:
            print(f"[-] Class '{cls}' not found in dump.cs")
            sys.exit(1)
        print(f"\n[+] Methods in '{cls}':")
        for mname, rvas in class_data[cls]["methods"].items():
            rva_str = ", ".join(f"0x{r:X}" for r in rvas)
            print(f"    {mname:30s} -> RVA: {rva_str}")
        return

    dll_path = find_game_assembly()
    if not os.path.exists(dll_path):
        print(f"[-] GameAssembly.dll not found at: {dll_path}")
        sys.exit(1)

    pe = pefile.PE(dll_path, fast_load=True)
    pe_data = pe.get_memory_mapped_image()
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    # Direct RVA + Offset mode
    if args.rva is not None and args.offset is not None:
        rva = args.rva
        offset = args.offset
        print(f"\n[*] Scanning RVA 0x{rva:X} for offset 0x{offset:X}...")
        matches = find_usages(pe_data, md, rva, offset, max_bytes=args.max_bytes)
        if matches:
            for m in matches:
                print(
                    f"[+] FOUND: offset 0x{offset:X} at instr_idx={m['instr_idx']}, op_idx={m['op_idx']} [{m['op_type']}] -> {m['instr_text']}"
                )
            if args.disasm:
                disassemble_method(
                    pe_data, md, rva, highlight_offset=offset, max_bytes=args.max_bytes
                )
            print_rule_snippets("field", "Class", "Class", "Method", offset, matches[0])
        else:
            print(
                f"[-] NOT FOUND: offset 0x{offset:X} not used in RVA 0x{rva:X} (scanned {args.max_bytes} bytes)"
            )
            if args.disasm:
                disassemble_method(pe_data, md, rva, max_bytes=args.max_bytes)
        return

    # Specific Class, Method, Field passed via arguments
    if args.class_name and args.method_name:
        meth_cls = args.class_name
        meth_name = args.method_name
        field_name = args.field_name
        field_cls = args.field_class if args.field_class else meth_cls

        rvas = class_data.get(meth_cls, {}).get("methods", {}).get(meth_name, [])
        if not rvas:
            print(f"[-] Method {meth_cls}::{meth_name} not found in dump.cs")
            sys.exit(1)

        rva = rvas[0]
        offset = None
        if field_name:
            offset = class_data.get(field_cls, {}).get("fields", {}).get(field_name)
            if offset is None:
                # Try prefix search or fuzzy match
                for fname, foff in (
                    class_data.get(field_cls, {}).get("fields", {}).items()
                ):
                    if field_name.lower() in fname.lower():
                        offset = foff
                        field_name = fname
                        print(
                            f"[*] Matched field '{field_name}' (0x{offset:X}) in {field_cls}"
                        )
                        break

            if offset is None:
                print(f"[-] Field '{field_name}' not found in class '{field_cls}'")
                sys.exit(1)

        print(f"\n[*] Target: {meth_cls}::{meth_name} (RVA 0x{rva:X})")
        if offset is not None:
            print(
                f"[*] Field: {field_cls}::{field_name} (Offset 0x{offset:X} / {offset})"
            )
            matches = find_usages(pe_data, md, rva, offset, max_bytes=args.max_bytes)
            if matches:
                for m in matches:
                    print(
                        f"[+] FOUND: instr_idx={m['instr_idx']}, op_idx={m['op_idx']} [{m['op_type']}] -> {m['instr_text']}"
                    )
                if args.disasm:
                    disassemble_method(
                        pe_data,
                        md,
                        rva,
                        highlight_offset=offset,
                        max_bytes=args.max_bytes,
                    )
                print_rule_snippets(
                    field_name, field_cls, meth_cls, meth_name, offset, matches[0]
                )
            else:
                print(
                    f"[-] Offset 0x{offset:X} was NOT found in instruction operands for {meth_cls}::{meth_name}"
                )
                if args.disasm:
                    disassemble_method(pe_data, md, rva, max_bytes=args.max_bytes)
        else:
            # Just disassemble if no field given
            disassemble_method(pe_data, md, rva, max_bytes=args.max_bytes)
        return

    # Default audit mode (either --all or no arguments)
    print("\n[*] Auditing all default extraction targets across project...")
    print("=" * 70)
    for meth_cls, meth_name, field_name, field_cls in DEFAULT_TARGETS:
        rvas = class_data.get(meth_cls, {}).get("methods", {}).get(meth_name, [])
        if not rvas:
            print(f"[-] Method {meth_cls}::{meth_name} NOT FOUND in dump.cs")
            continue

        rva = rvas[0]
        offset = class_data.get(field_cls, {}).get("fields", {}).get(field_name)
        if offset is None:
            print(f"[-] Field {field_cls}::{field_name} NOT FOUND in dump.cs")
            continue

        matches = find_usages(pe_data, md, rva, offset)
        if matches:
            m = matches[0]
            print(
                f"[+] FOUND: {field_cls}::{field_name} (0x{offset:02X}) in {meth_cls}::{meth_name} at instr_idx={m['instr_idx']}, op_idx={m['op_idx']} ({m['instr_text']})"
            )
        else:
            print(
                f"[-] NOT IN MEM/IMM: {field_cls}::{field_name} (0x{offset:02X}) in {meth_cls}::{meth_name} (RVA 0x{rva:X})"
            )
    print("=" * 70)
    print(
        "[*] Tip: Run with `python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> --disasm` for deep inspection."
    )


if __name__ == "__main__":
    main()
