import os
import re
import pefile
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM

DUMP_PATH = os.path.join(os.path.dirname(__file__), "../dumped/dump.cs")
DLL_PATH = os.path.join(os.path.dirname(__file__), "../dumped/GameAssembly.dll")

def get_offsets():
    class_data = {}
    current_class = None

    with open(DUMP_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    class_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract)\s+)*(?:class|struct|interface|enum)\s+(\w+)")
    rva_regex = re.compile(r"// RVA: 0x([0-9A-Fa-f]+)")
    method_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract|override|virtual)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+|\.ctor)\s*\(")
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
                class_data[current_class]['fields'][f_match.group(1)] = int(f_match.group(2), 16)
                continue

            rva_match = rva_regex.search(line)
            if rva_match and i + 1 < len(lines):
                m_match = method_regex.search(lines[i+1])
                if m_match:
                    method_name = m_match.group(1)
                    rva_val = int(rva_match.group(1), 16)
                    if method_name not in class_data[current_class]['methods']:
                        class_data[current_class]['methods'][method_name] = []
                    class_data[current_class]['methods'][method_name].append(rva_val)

    return class_data

def find_usage(pe, pe_data, md, rva, target_offset):
    code = pe_data[rva:rva+800]
    for i, instr in enumerate(md.disasm(code, rva)):
        for op_idx, op in enumerate(instr.operands):
            if op.type == CS_OP_MEM:
                if op.mem.disp == target_offset:
                    return i, op_idx, instr.mnemonic
    return None, None, None

def main():
    class_data = get_offsets()
    pe = pefile.PE(DLL_PATH)
    pe_data = pe.get_memory_mapped_image()
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    targets = [
        ("AttackReceiver", "Recieve", "isPlayer"),
        ("Attack", ".ctor", "AttackDamage"),
        ("MovementControl", "Move", "_view"),
        ("MovementControl", "Move", "networkPlayerSync", "PlayerCharacter"),
        ("MovementControl", "Move", "_isNet", "NetworkPlayerSync"),
        ("SteamPurchaseService", "InitiatePurchase", "_rewarder"),
        ("EnemyNpcController", "Update", "alive"),
    ]

    for item in targets:
        meth_cls = item[0]
        meth_name = item[1]
        field_name = item[2]
        field_cls = item[3] if len(item) > 3 else meth_cls

        rvas = class_data.get(meth_cls, {}).get('methods', {}).get(meth_name, [])
        if not rvas:
            print(f"Method {meth_cls}::{meth_name} not found")
            continue

        offset = class_data.get(field_cls, {}).get('fields', {}).get(field_name, None)
        if offset is None:
            print(f"Field {field_cls}::{field_name} not found")
            continue

        rva = rvas[0]
        idx, op_idx, mnemonic = find_usage(pe, pe_data, md, rva, offset)
        if idx is not None:
            print(f"FOUND: {field_cls}::{field_name} (0x{offset:X}) in {meth_cls}::{meth_name} at instr_idx={idx}, op_idx={op_idx} ({mnemonic})")
        else:
            print(f"NOT FOUND: {field_cls}::{field_name} (0x{offset:X}) in {meth_cls}::{meth_name}")

if __name__ == '__main__':
    main()
