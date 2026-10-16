#!/usr/bin/env python3
"""
Evitania Deobfuscator & Symbol Bridge
Reconciles obfuscated IL2CPP dumps using stable AOB signatures:
1. Matches known canonical signatures in GameAssembly.dll to discover current RVAs.
2. Cross-references matched RVAs with Il2CppDumper's script.json to map obfuscated names.
3. Correlates field offsets in dump.cs to deobfuscate field names.
4. Generates:
   - deobfuscation_map.json (symbol translation database)
   - rename_ghidra.py (Ghidra auto-renamer)
   - rename_ida.py (IDA Pro auto-renamer)
   - dump_deobfuscated.cs (Clean, readable C# dump)
"""

import argparse
import json
import os
import re
import sys
import time
import pefile

# Add Tools directory to sys.path
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../.."))
sys.path.insert(0, SCRIPT_DIR)

from aobgenerator import find_game_assembly, parse_pattern_to_regex

CONFIG_PATH = os.path.join(PROJECT_ROOT, "config.json")
DUMP_DIR = os.path.join(PROJECT_ROOT, "resources", "dumped")
SCRIPT_JSON_PATH = os.path.join(DUMP_DIR, "script.json")
DUMP_CS_PATH = os.path.join(DUMP_DIR, "dump.cs")
MAP_OUTPUT_PATH = os.path.join(DUMP_DIR, "deobfuscation_map.json")


def print_banner(title):
    print("\n" + "=" * 65)
    print(f"  {title.center(61)}")
    print("=" * 65)


def parse_canonical_key(key):
    """
    Parses a method key from config.json (e.g. 'EnemyNpcController_TakeDamage' or 'CurrencyService_Subtract_1')
    Returns: (canonical_class, canonical_method)
    """
    parts = key.split("_")
    if len(parts) >= 2:
        cls = parts[0]
        # Remove trailing overload index if present (e.g. Subtract_1 -> Subtract)
        if parts[-1].isdigit():
            meth = "_".join(parts[1:-1])
        else:
            meth = "_".join(parts[1:])
        return cls, meth
    return key, key


def load_dump_fields(dump_path):
    """
    Parses dump.cs and returns a dict: { class_name: { field_name: offset_int } }
    """
    if not os.path.exists(dump_path):
        return {}

    class_fields = {}
    current_class = None

    class_regex = re.compile(
        r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract)\s+)*(?:class|struct|interface|enum)\s+([<>\w.]+)"
    )
    field_regex = re.compile(
        r"^\s*(?:(?:public|private|protected|internal|static|readonly|const|volatile)\s+)*.*?\s+([<>\w]+|[a-zA-Z0-9_]+);\s*//\s*0x([0-9A-Fa-f]+)"
    )

    with open(dump_path, "r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            c_match = class_regex.search(line)
            if c_match:
                current_class = c_match.group(1)
                if current_class not in class_fields:
                    class_fields[current_class] = {}
                continue

            if current_class:
                f_match = field_regex.search(line)
                if f_match:
                    fname = f_match.group(1)
                    foff = int(f_match.group(2), 16)
                    class_fields[current_class][fname] = foff

    return class_fields


def load_extracted_offsets(offsets_hpp_path):
    offsets = {}
    if not os.path.exists(offsets_hpp_path):
        return offsets
    regex = re.compile(r"constexpr\s+int\s+(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+);")
    with open(offsets_hpp_path, "r", encoding="utf-8") as f:
        for line in f:
            m = regex.search(line)
            if m:
                offsets[m.group(1)] = int(m.group(2), 0)
    return offsets


def build_bridge(dll_path, config_path, script_json_path, dump_cs_path=None):
    """
    Scans GameAssembly.dll with config.json signatures, matches RVAs with script.json,
    and returns a full deobfuscation mapping dictionary.
    """
    if not os.path.exists(dll_path):
        print(f"[-] Error: GameAssembly.dll not found at {dll_path}")
        return None

    if not os.path.exists(config_path):
        print(f"[-] Error: config.json not found at {config_path}")
        return None

    if not os.path.exists(script_json_path):
        print(f"[-] Error: script.json not found at {script_json_path}")
        print("[-] Please run Il2CppDumper and place script.json in resources/dumped/")
        return None

    print("[*] Loading GameAssembly.dll...")
    pe = pefile.PE(dll_path, fast_load=True)
    pe_data = pe.get_memory_mapped_image()

    print("[*] Loading config.json...")
    with open(config_path, "r", encoding="utf-8") as f:
        config = json.load(f)
    sig_db = config.get("signatures", {})

    print("[*] Loading script.json...")
    with open(script_json_path, "r", encoding="utf-8") as f:
        script = json.load(f)

    # Index script.json by RVA (Address)
    methods_by_addr = {}
    for m in script.get("ScriptMethod", []):
        methods_by_addr[m["Address"]] = m

    print(f"[+] Indexed {len(methods_by_addr)} methods from script.json")

    deobf_map = {
        "generated_at": int(time.time()),
        "game_version": config.get("version", "unknown"),
        "steam_build_id": config.get("steam_build_id", "unknown"),
        "classes": {},  # { obfuscated_class: canonical_class }
        "methods": {},  # { obfuscated_full_name: canonical_full_name }
        "fields": {},  # { "obfuscated_class::obfuscated_field": "canonical_class::canonical_field" }
        "rva_map": {},  # { "0xRVA": { "canonical_key": ..., "obfuscated_name": ... } }
    }

    print("\n--- Correlating Signatures with script.json ---")
    matched_count = 0
    for key, data in sig_db.items():
        pattern = data.get("signature")
        if not pattern or key.startswith("Il2Cpp_"):
            continue

        canon_class, canon_method = parse_canonical_key(key)
        regex_str = parse_pattern_to_regex(pattern)
        regex = re.compile(regex_str, re.DOTALL)

        matches = list(regex.finditer(pe_data))
        if len(matches) == 1:
            rva = matches[0].start()
            script_entry = methods_by_addr.get(rva)
            if script_entry:
                raw_name = script_entry["Name"]
                matched_count += 1

                # Parse class and method from script.json (e.g. Namespace.Class$$Method)
                if "$$" in raw_name:
                    raw_cls, raw_meth = raw_name.split("$$", 1)
                else:
                    raw_cls, raw_meth = raw_name, raw_name

                # Remove namespace for short class name if needed
                short_raw_cls = raw_cls.split(".")[-1]

                deobf_map["classes"][raw_cls] = canon_class
                deobf_map["classes"][short_raw_cls] = canon_class
                deobf_map["methods"][raw_name] = f"{canon_class}$${canon_method}"
                deobf_map["rva_map"][f"0x{rva:X}"] = {
                    "canonical_key": key,
                    "canonical_class": canon_class,
                    "canonical_method": canon_method,
                    "obfuscated_name": raw_name,
                    "obfuscated_class": raw_cls,
                }

                print(f"[+] {key:40s} -> RVA 0x{rva:07X} <===> {raw_name}")
            else:
                print(
                    f"[~] {key:40s} -> RVA 0x{rva:07X} (Matched binary, but missing in script.json)"
                )
        elif len(matches) == 0:
            print(f"[-] {key:40s} -> 0 matches in GameAssembly.dll (Broken signature)")
        else:
            print(f"[-] {key:40s} -> {len(matches)} matches (Ambiguous signature)")

    # Field Correlation via dump.cs
    if dump_cs_path and os.path.exists(dump_cs_path):
        print("\n[*] Correlating field offsets from dump.cs...")
        dump_fields = load_dump_fields(dump_cs_path)
        offsets_hpp_path = os.path.join(
            PROJECT_ROOT, "src", "Modules", "Hooks", "Offsets.hpp"
        )
        extracted_offsets = load_extracted_offsets(offsets_hpp_path)

        for key, data in sig_db.items():
            extract_rules = data.get("extract", {})
            if not extract_rules:
                continue

            canon_class, _ = parse_canonical_key(key)

            # Find matching obfuscated class names
            target_raw_classes = [
                raw_c
                for raw_c, c_cls in deobf_map["classes"].items()
                if c_cls == canon_class
            ]
            for raw_cls in target_raw_classes:
                raw_cls_fields = dump_fields.get(raw_cls, {})
                for field_name, rule in extract_rules.items():
                    target_offset = rule.get("hardcoded")
                    if target_offset is None:
                        target_offset = extracted_offsets.get(field_name)
                    if target_offset is None:
                        continue

                    # Find which field in raw_cls has target_offset
                    for r_fname, r_off in raw_cls_fields.items():
                        if r_off == target_offset:
                            raw_field_key = f"{raw_cls}::{r_fname}"
                            canon_field_key = f"{canon_class}::{field_name}"
                            deobf_map["fields"][raw_field_key] = canon_field_key
                            deobf_map["fields"][r_fname] = field_name
                            print(
                                f"[+] Field: {raw_cls}::{r_fname} (0x{target_offset:X}) <===> {canon_class}::{field_name}"
                            )

    print(f"\n[+] Total Methods Correlated: {matched_count}")
    print(f"[+] Total Classes Mapped:     {len(set(deobf_map['classes'].values()))}")
    print(f"[+] Total Fields Mapped:      {len(deobf_map['fields'])}")
    return deobf_map


def export_ghidra_script(deobf_map, output_path):
    lines = [
        "# Ghidra Auto-Renamer Script for Evitania",
        "# Generated automatically by deobfuscator.py",
        "# Run inside Ghidra's Script Manager (Python 3 / Jython)",
        "",
        "from ghidra.program.model.symbol import SourceType",
        "",
        "currentProgram = getCurrentProgram()",
        "symbolTable = currentProgram.getSymbolTable()",
        "listing = currentProgram.getListing()",
        "",
        "RENAME_MAP = {",
    ]

    for rva_str, info in deobf_map.get("rva_map", {}).items():
        canon = f"{info['canonical_class']}__{info['canonical_method']}"
        lines.append(f'    {rva_str}: "{canon}",')

    lines.extend(
        [
            "}",
            "",
            "base_addr = currentProgram.getImageBase().getOffset()",
            "for rva, name in RENAME_MAP.items():",
            "    addr = currentProgram.getAddressFactory().getDefaultAddressSpace().getAddress(base_addr + rva)",
            "    func = listing.getFunctionAt(addr)",
            "    if func is not None:",
            "        old_name = func.getName()",
            "        func.setName(name, SourceType.USER_DEFINED)",
            "        print(f'[+] Renamed {old_name} -> {name} at {addr}')",
            "    else:",
            "        symbolTable.createLabel(addr, name, SourceType.USER_DEFINED)",
            "        print(f'[+] Created label {name} at {addr}')",
            "",
            "print('[+] Ghidra symbol renaming completed!')",
        ]
    )

    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print(f"[+] Exported Ghidra script -> {output_path}")


def export_ida_script(deobf_map, output_path):
    lines = [
        "# IDA Pro Auto-Renamer Script for Evitania",
        "# Generated automatically by deobfuscator.py",
        "# Run in IDA via File -> Script File",
        "",
        "import idc",
        "import ida_name",
        "",
        "base_addr = idc.get_imagebase()",
        "RENAME_MAP = {",
    ]

    for rva_str, info in deobf_map.get("rva_map", {}).items():
        canon = f"{info['canonical_class']}__{info['canonical_method']}"
        lines.append(f'    {rva_str}: "{canon}",')

    lines.extend(
        [
            "}",
            "",
            "for rva, name in RENAME_MAP.items():",
            "    addr = base_addr + rva",
            "    ida_name.set_name(addr, name, ida_name.SN_NOWARN)",
            "    print(f'[+] Renamed 0x{addr:X} -> {name}')",
            "",
            "print('[+] IDA Pro symbol renaming completed!')",
        ]
    )

    with open(output_path, "w", encoding="utf-8") as f:
        f.write("\n".join(lines))
    print(f"[+] Exported IDA Pro script -> {output_path}")


def generate_deobfuscated_dump(dump_path, deobf_map, output_path):
    if not os.path.exists(dump_path):
        print(f"[-] Cannot deobfuscate dump: {dump_path} not found.")
        return

    print(f"[*] Generating deobfuscated C# dump -> {output_path}...")
    class_map = deobf_map.get("classes", {})
    field_map = deobf_map.get("fields", {})

    with (
        open(dump_path, "r", encoding="utf-8", errors="ignore") as f_in,
        open(output_path, "w", encoding="utf-8") as f_out,
    ):
        for line in f_in:
            replaced_line = line
            # Replace mapped class names
            for raw_cls, canon_cls in class_map.items():
                if raw_cls in replaced_line and raw_cls != canon_cls:
                    replaced_line = re.sub(
                        rf"\b{re.escape(raw_cls)}\b",
                        f"{canon_cls} /* was {raw_cls} */",
                        replaced_line,
                    )

            # Replace mapped field names
            for raw_field, canon_field in field_map.items():
                if (
                    "::" not in raw_field
                    and raw_field in replaced_line
                    and raw_field != canon_field
                ):
                    replaced_line = re.sub(
                        rf"\b{re.escape(raw_field)}\b",
                        f"{canon_field} /* was {raw_field} */",
                        replaced_line,
                    )

            f_out.write(replaced_line)

    print(f"[+] Successfully generated deobfuscated dump: {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description="Evitania Deobfuscator & Symbol Bridge - Maps obfuscated symbols using AOB signatures."
    )
    parser.add_argument(
        "--dll",
        default=None,
        help="Path to GameAssembly.dll (defaults to auto-detected Steam path)",
    )
    parser.add_argument(
        "--config",
        default=CONFIG_PATH,
        help=f"Path to config.json (default: {CONFIG_PATH})",
    )
    parser.add_argument(
        "--script-json",
        default=SCRIPT_JSON_PATH,
        help=f"Path to script.json (default: {SCRIPT_JSON_PATH})",
    )
    parser.add_argument(
        "--dump-cs",
        default=DUMP_CS_PATH,
        help=f"Path to dump.cs (default: {DUMP_CS_PATH})",
    )
    parser.add_argument(
        "--out-map",
        default=MAP_OUTPUT_PATH,
        help=f"Path to save deobfuscation_map.json (default: {MAP_OUTPUT_PATH})",
    )
    parser.add_argument(
        "--ghidra", action="store_true", help="Export Ghidra rename Python script"
    )
    parser.add_argument(
        "--ida", action="store_true", help="Export IDA Pro rename Python script"
    )
    parser.add_argument(
        "--patch-dump",
        action="store_true",
        help="Produce a deobfuscated dump_deobfuscated.cs file",
    )
    parser.add_argument(
        "--all",
        action="store_true",
        help="Run full pipeline: generate map, Ghidra script, IDA script, and deobfuscated dump",
    )

    args = parser.parse_args()

    print_banner("EVITANIA DEOBFUSCATOR & SYMBOL BRIDGE")

    dll_path = args.dll or find_game_assembly()
    deobf_map = build_bridge(dll_path, args.config, args.script_json, args.dump_cs)

    if not deobf_map:
        sys.exit(1)

    # Save mapping JSON
    os.makedirs(os.path.dirname(args.out_map), exist_ok=True)
    with open(args.out_map, "w", encoding="utf-8") as f:
        json.dump(deobf_map, f, indent=2)
    print(f"\n[+] Saved Deobfuscation Map -> {args.out_map}")

    # Ghidra script
    if args.ghidra or args.all:
        ghidra_path = os.path.join(DUMP_DIR, "rename_ghidra.py")
        export_ghidra_script(deobf_map, ghidra_path)

    # IDA script
    if args.ida or args.all:
        ida_path = os.path.join(DUMP_DIR, "rename_ida.py")
        export_ida_script(deobf_map, ida_path)

    # Patch dump.cs
    if args.patch_dump or args.all:
        clean_dump_path = os.path.join(DUMP_DIR, "dump_deobfuscated.cs")
        generate_deobfuscated_dump(args.dump_cs, deobf_map, clean_dump_path)

    print("\n[+] Deobfuscation bridge completed successfully!")


if __name__ == "__main__":
    main()
