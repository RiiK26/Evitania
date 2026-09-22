import os
import re
import pefile
import platform
from capstone import Cs, CS_ARCH_X86, CS_MODE_64, CS_OP_MEM, CS_OP_IMM

DUMP_PATH = os.path.join(os.path.dirname(__file__), "../dumped/dump.cs")

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
