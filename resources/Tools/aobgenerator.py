import os
import sys
import re
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_IMM, CS_OP_MEM

DUMP_PATH = os.path.join(os.path.dirname(__file__), "../dumped/dump.cs")
DLL_PATH = os.path.join(os.path.dirname(__file__), "../dumped/GameAssembly.dll")

def get_offsets():
    print("Parsing dump.cs...")
    class_data = {}
    current_class = None

    if not os.path.exists(DUMP_PATH):
        print(f"Error: Could not find dump.cs at {DUMP_PATH}")
        return None

    with open(DUMP_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    class_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract)\s+)*(?:class|struct|interface|enum)\s+(\w+)")
    rva_regex = re.compile(r"// RVA: 0x([0-9A-Fa-f]+)")
    method_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract|override|virtual)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+)\s*\(")
    field_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|readonly|const|volatile)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+);\s*//\s*0x([0-9A-Fa-f]+)")

    for i, line in enumerate(lines):
        c_match = class_regex.search(line)
        if c_match:
            current_class = c_match.group(1)
            if current_class not in class_data:
                class_data[current_class] = {'methods': {}, 'fields': {}}
            continue

        if current_class:
            f_match = field_regex.search(line)
            if f_match:
                class_data[current_class]['fields'][f_match.group(1)] = f_match.group(2)
                continue

            rva_match = rva_regex.search(line)
            if rva_match and i + 1 < len(lines):
                m_match = method_regex.search(lines[i+1])
                if m_match:
                    method_name = m_match.group(1)
                    rva_val = rva_match.group(1)
                    if method_name not in class_data[current_class]['methods']:
                        class_data[current_class]['methods'][method_name] = []
                    class_data[current_class]['methods'][method_name].append(rva_val)

    return class_data

def generate_signature(pe, md, rva, min_length=15):
    # Read bytes at RVA
    offset = pe.get_offset_from_rva(rva)
    if offset == 0:
        return None

    # Read a chunk of bytes to disassemble
    code = pe.get_memory_mapped_image()[rva:rva+100]

    sig_bytes = []

    for i in md.disasm(code, rva):
        instr_bytes = i.bytes

        # Heuristic: Wildcard relative offsets and hardcoded absolute addresses
        # CALL / JMP rel32
        if i.mnemonic in ["call", "jmp"] and len(instr_bytes) == 5:
            sig_bytes.extend([f"{instr_bytes[0]:02X}", "?", "?", "?", "?"])
        # RIP-relative addressing (e.g. MOV RAX, [RIP + disp32]) or absolute immediate (e.g. MOV EAX, imm32)
        else:
            has_disp32 = False
            for op in i.operands:
                if op.type == CS_OP_MEM and op.mem.base == 0: # Absolute address
                    has_disp32 = True
                elif op.type == CS_OP_IMM and len(instr_bytes) >= 5: # Immediate
                    has_disp32 = True

            if has_disp32 and len(instr_bytes) >= 5:
                # Keep first few bytes (opcode/ModRM), wildcard the immediate 4 bytes
                prefix_len = len(instr_bytes) - 4
                for b in instr_bytes[:prefix_len]:
                    sig_bytes.append(f"{b:02X}")
                sig_bytes.extend(["?"] * 4)
            else:
                for b in instr_bytes:
                    sig_bytes.append(f"{b:02X}")

        if len(sig_bytes) >= min_length:
            # We must end on a full instruction boundary, so we can stop here
            break

    return " ".join(sig_bytes)

def main():
    if not os.path.exists(DLL_PATH):
        print(f"Error: GameAssembly.dll not found at {DLL_PATH}")
        print("Please copy GameAssembly.dll to the resources/dumped folder.")
        return

    class_data = get_offsets()
    if not class_data:
        return

    print("Loading GameAssembly.dll...")
    pe = pefile.PE(DLL_PATH)
    pe.parse_data_directories()

    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    targets = [
        ("CurrencyService", "Subtract"),
        ("Skill", "AddExperience"),
        ("SteamPurchaseService", "InitiatePurchase"),
        ("MobilePurchaseService", "InitiatePurchase"),
        ("SteamPurchaseService", "FindLot"),
        ("MobilePurchaseService", "FindLot"),
        ("IAPRewarder", "Reward"),
        ("MovementControl", "Move"),
        ("Time", "set_timeScale"),
        ("SpawnPortal", "CurrentSpawnInterval"),
        ("GatheringService", "GetSpeed")
    ]

    print("\n--- Generated AOB Signatures ---")

    signatures_content = [
        "#pragma once",
        "",
        "namespace Signatures",
        "{",
    ]

    for cls, method in targets:
        if cls in class_data and method in class_data[cls]['methods']:
            rva_hex = class_data[cls]['methods'][method][0]
            rva = int(rva_hex, 16)

            sig = generate_signature(pe, md, rva)
            if sig:
                print(f"[+] {cls}::{method} -> {sig}")

                # Format for C++ header
                safe_cls = cls
                safe_method = method.replace("set_", "").replace("get_", "")

                signatures_content.append(f"  // 0x{rva_hex}")
                signatures_content.append(f"  constexpr const char* {safe_cls}_{safe_method} = \"{sig}\";")
                signatures_content.append("")
            else:
                print(f"[-] Failed to generate signature for {cls}::{method}")
        else:
            print(f"[-] Could not find {cls}::{method} in dump")

    signatures_content.append("}  // namespace Signatures")

    sig_path = os.path.join(os.path.dirname(__file__), "../../src/Modules/Hooks/Signatures.hpp")
    with open(sig_path, "w") as f:
        f.write("\n".join(signatures_content))

    print(f"\nSignatures saved to {sig_path}")

if __name__ == "__main__":
    main()
