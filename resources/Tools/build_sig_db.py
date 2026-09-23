import os
import re
import json
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

def parse_pattern_to_regex(pattern):
    regex_str = b""
    for b in pattern.split():
        if b == '?':
            regex_str += b"."
        else:
            regex_str += re.escape(bytes([int(b, 16)]))
    return regex_str

def is_unique_signature(pe_data, pattern):
    regex_str = parse_pattern_to_regex(pattern)
    regex = re.compile(regex_str, re.DOTALL)
    matches = 0
    for _ in regex.finditer(pe_data):
        matches += 1
        if matches > 1:
            return False
    return matches == 1

def generate_signature(pe, md, pe_data, rva, initial_min_length=15):
    offset = pe.get_offset_from_rva(rva)
    if offset == 0:
        return None

    code = pe_data[rva:rva+400]

    current_min_length = initial_min_length
    while current_min_length < 300:
        sig_bytes = []
        for i in md.disasm(code, rva):
            instr_bytes = i.bytes

            # Heuristic: Wildcard relative offsets and hardcoded absolute addresses
            if i.mnemonic in ["call", "jmp"] and len(instr_bytes) == 5:
                sig_bytes.extend([f"{instr_bytes[0]:02X}", "?", "?", "?", "?"])
            else:
                wildcards = [False] * len(instr_bytes)

                has_rel_or_abs = False
                for op in i.operands:
                    if op.type == CS_OP_MEM and op.mem.base in (0, 41):
                        has_rel_or_abs = True

                if has_rel_or_abs and getattr(i, "disp_size", 0) >= 4:
                    disp_off = getattr(i, "disp_offset", 0)
                    for j in range(disp_off, disp_off + i.disp_size):
                        if j < len(wildcards):
                            wildcards[j] = True

                if getattr(i, "imm_size", 0) >= 4:
                    imm_off = getattr(i, "imm_offset", 0)
                    for j in range(imm_off, imm_off + i.imm_size):
                        if j < len(wildcards):
                            wildcards[j] = True

                for j, b in enumerate(instr_bytes):
                    if wildcards[j]:
                        sig_bytes.append("?")
                    else:
                        sig_bytes.append(f"{b:02X}")

            if len(sig_bytes) >= current_min_length:
                break

        sig = " ".join(sig_bytes)
        if is_unique_signature(pe_data, sig):
            return sig

        current_min_length += 5

    return None

def find_usage(pe, pe_data, md, rva, target_offset):
    code = pe_data[rva:rva+800]
    for i, instr in enumerate(md.disasm(code, rva)):
        for op_idx, op in enumerate(instr.operands):
            if op.type == CS_OP_MEM:
                if op.mem.disp == target_offset:
                    return i, op_idx
    return None, None

def main():
    class_data = get_offsets()
    pe = pefile.PE(DLL_PATH)
    pe_data = pe.get_memory_mapped_image()
    md = Cs(CS_ARCH_X86, CS_MODE_64)
    md.detail = True

    # Master list of what we want in our signature database
    # (ClassName, MethodName, OverloadIdx, [(FieldName, FieldClass)])
    targets = [
        ("EnemyNpcController", "TakeDamage", 0, [("alive", "EnemyNpcController")]),
        ("EnemyNpcController", "Update", 0, [("alive", "EnemyNpcController")]),
        ("AttackReceiver", "Recieve", 0, [("AttackDamage", "Attack")]),
        ("RealmAttackReceiver", "Recieve", 0, []),
        ("EasterAttackReceiver", "Recieve", 0, []),
        ("AttackReceiver", "Awake", 0, [("isPlayer", "AttackReceiver")]),
        ("CurrencyService", "Subtract", 0, []),
        ("CurrencyService", "Subtract", 1, []),
        ("Skill", "AddExperience", 0, []),
        ("SteamPurchaseService", "InitiatePurchase", 0, [("_rewarder", "SteamPurchaseService")]),
        ("MobilePurchaseService", "InitiatePurchase", 0, [("_rewarder", "SteamPurchaseService")]),
        ("SteamPurchaseService", "FindLot", 0, []),
        ("IAPRewarder", "Reward", 0, []),
        ("MovementControl", "Move", 0, [("_view", "MovementControl"), ("networkPlayerSync", "PlayerCharacter"), ("_isNet", "NetworkPlayerSync")]),
        ("Time", "set_timeScale", 0, []),
        ("TimeskipItem", "get_CanUseImpl", 0, []),
        ("HourglassService", "CostFactor", 0, []),
        ("HourglassService", "UpgradeCost", 0, []),
        ("HourglassService", "LevelCost", 0, []),
        ("HourglassUpgradeItem", "GetCost", 0, []),
        ("HourglassHardUUpgrade", "GetCostAmount", 0, []),
        ("HourglassService", "BuyUpgrade", 0, []),
        ("HourglassService", "BuyHardUpgrade", 0, []),
        ("HourglassService", "BuyTalent", 0, []),
        ("HourglassService", "BuyGenerator", 0, []),
        ("HourglassService", "BuyUpgradeBlock", 0, []),
        ("AntiCheatService", "Initialize", 0, []),
        ("AntiCheatService", "OnSpeedHackDetected", 0, []),
        ("AntiCheatService", "OnObscuredCheatingDetected", 0, []),
        ("AntiCheatService", "Handle", 0, []),
        ("AntiCheatService", "Apply", 0, []),
        ("AntiCheatService", "ReportCheatToAnalytics", 0, []),
        ("Attack", ".ctor", 0, [("AttackDamage", "Attack")]),
        ("PlayerCharacter", ".ctor", 0, [("networkPlayerSync", "PlayerCharacter")]),
        ("CurioGachaService", "RollRarity", 0, []),
        ("CurioPowerService", "GetLevelUpCost", 0, []),
        ("EngineerService", "TryGetUpgradeCost", 0, []),
        ("EngineerUpgradeConfig", "GetPrice", 0, []),
        ("MarketLotScriptableObject", "CurrentPrice", 0, []),
        ("MarketLot", "GetCurrentPrice", 0, []),
        ("PlayerCharacter", "TakeDamage", 0, []),
        ("HourglassService", "BuyCoinCdUpgrade", 0, []),
        ("HourglassService", "BuyCoinRewardUpgrade", 0, []),
        ("HourglassService", "BuyRiftWorker", 0, []),
        ("HourglassService", "BuyShopItem", 0, []),
        ("HourglassService", "CoinCdCost", 0, []),
        ("HourglassService", "CoinRewardCost", 0, []),
        ("HourglassService", "ShopNextCost", 0, []),
        ("HourglassLevelConfig", "SandCost", 0, []),
        ("HourglassLevelConfig", "GoldCost", 0, []),
    ]

    sig_db = {}

    for cls, method, overload_idx, fields_to_extract in targets:
        if cls in class_data and method in class_data[cls]['methods']:
            if overload_idx < len(class_data[cls]['methods'][method]):
                rva = class_data[cls]['methods'][method][overload_idx]
                sig = generate_signature(pe, md, pe_data, rva)

                if not sig:
                    print(f"[-] Could not generate sig for {cls}::{method}")
                    continue

                method_key = f"{cls}_{method}_{overload_idx}" if overload_idx > 0 else f"{cls}_{method}"

                extract_rules = {}
                for field_name, field_cls in fields_to_extract:
                    offset = class_data.get(field_cls, {}).get('fields', {}).get(field_name)
                    if offset is not None:
                        idx, op_idx = find_usage(pe, pe_data, md, rva, offset)
                        if idx is not None:
                            extract_rules[field_name] = {"instr_idx": idx, "op_idx": op_idx}
                        else:
                            # Fallback: Just record the hardcoded offset for fields we can't find via Capstone
                            extract_rules[field_name] = {"hardcoded": offset}

                sig_db[method_key] = {
                    "signature": sig,
                    "extract": extract_rules
                }
                print(f"[+] Added {method_key} to database")

    config_path = os.path.join(os.path.dirname(__file__), "../../config.json")

    # Load existing config if it exists to preserve version
    config = {"version": "unknown", "signatures": {}}
    if os.path.exists(config_path):
        with open(config_path, "r") as f:
            try:
                config = json.load(f)
            except json.JSONDecodeError:
                pass

    config["signatures"] = sig_db

    with open(config_path, "w") as f:
        json.dump(config, f, indent=4)
    print("Database built successfully -> config.json")

if __name__ == '__main__':
    main()
