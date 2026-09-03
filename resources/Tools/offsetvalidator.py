import re
import os
import sys

DUMP_PATH = os.path.join(os.path.dirname(__file__), "../dumped/dump.cs")
OFFSETS_PATH = os.path.join(os.path.dirname(__file__), "../../src/Modules/Hooks/Offsets.hpp")

def main():
    print("Parsing dump.cs...")
    class_data = {}
    current_class = None

    if not os.path.exists(DUMP_PATH):
        print(f"Error: Could not find dump.cs at {DUMP_PATH}")
        return

    with open(DUMP_PATH, "r", encoding="utf-8") as f:
        lines = f.readlines()

    class_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract)\s+)*(?:class|struct|interface|enum)\s+(\w+)")
    rva_regex = re.compile(r"// RVA: 0x([0-9A-Fa-f]+)")
    method_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|sealed|abstract|override|virtual)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+)\s*\(")
    field_regex = re.compile(r"^\s*(?:(?:public|private|protected|internal|static|readonly|const|volatile)\s+)*.*?\s+(\w+|[a-zA-Z0-9_]+);\s*//\s*0x([0-9A-Fa-f]+)")

    for i, line in enumerate(lines):
        # Handle Class
        c_match = class_regex.search(line)
        if c_match:
            current_class = c_match.group(1)
            if current_class not in class_data:
                class_data[current_class] = {'methods': {}, 'fields': {}}
            continue

        if current_class:
            # Handle Field
            f_match = field_regex.search(line)
            if f_match:
                class_data[current_class]['fields'][f_match.group(1)] = f_match.group(2)
                continue

            # Handle Method
            rva_match = rva_regex.search(line)
            if rva_match and i + 1 < len(lines):
                rva = rva_match.group(1)
                next_line = lines[i+1]
                m_match = method_regex.search(next_line)
                if m_match:
                    m_name = m_match.group(1)
                    if m_name not in class_data[current_class]['methods']:
                        class_data[current_class]['methods'][m_name] = []
                    class_data[current_class]['methods'][m_name].append(rva)

    print(f"Parsed {len(class_data)} classes/structs.")

    if not os.path.exists(OFFSETS_PATH):
        print(f"Error: Could not find Offsets.hpp at {OFFSETS_PATH}")
        return

    print("Checking Offsets.hpp...")
    with open(OFFSETS_PATH, "r", encoding="utf-8") as f:
        offset_lines = f.readlines()

    new_offset_lines = []
    current_namespace = []
    updated_count = 0
    valid_count = 0
    not_found_count = 0

    # ANSI Colors
    C_GREEN = '\033[92m'
    C_YELLOW = '\033[93m'
    C_RED = '\033[91m'
    C_END = '\033[0m'

    for line in offset_lines:
        ns_match = re.search(r"^\s*namespace\s+(\w+)", line)
        if ns_match:
            current_namespace.append(ns_match.group(1))
        elif re.search(r"^\s*}", line) and current_namespace:
            current_namespace.pop()

        var_match = re.search(r"constexpr\s+uintptr_t\s+(\w+)\s*=\s*0x([0-9A-Fa-f]+);", line)
        if var_match and current_namespace:
            var_name = var_match.group(1)
            old_val = var_match.group(2)

            is_field = ("Fields" in current_namespace)
            cls_name = current_namespace[-1]
            target_name = var_name

            # Heuristics for mixed names
            if not is_field and "_" in var_name:
                parts = var_name.split("_", 1)
                if parts[0] in class_data:
                    cls_name = parts[0]
                    target_name = parts[1]

            new_val = None
            if cls_name in class_data:
                if is_field:
                    if target_name in class_data[cls_name]['fields']:
                        new_val = class_data[cls_name]['fields'][target_name]
                else:
                    if target_name in class_data[cls_name]['methods']:
                        # Prefer the first overload
                        new_val = class_data[cls_name]['methods'][target_name][0]

            if new_val:
                if new_val.upper() != old_val.upper():
                    print(f"{C_YELLOW}[UPDATED]{C_END} {cls_name}::{target_name} -> 0x{new_val.upper()} (was 0x{old_val})")
                    line = line.replace(f"0x{old_val}", f"0x{new_val.upper()}")
                    updated_count += 1
                else:
                    print(f"{C_GREEN}[VALID]{C_END}   {cls_name}::{target_name} -> 0x{old_val}")
                    valid_count += 1
            else:
                print(f"{C_RED}[MISSING]{C_END} Could not verify {cls_name}::{target_name} (0x{old_val})")
                not_found_count += 1

        new_offset_lines.append(line)

    with open(OFFSETS_PATH, "w", encoding="utf-8") as f:
        f.writelines(new_offset_lines)

    print("\n--- Validation Summary ---")
    print(f"{C_GREEN}Valid:{C_END}   {valid_count}")
    print(f"{C_YELLOW}Updated:{C_END} {updated_count}")
    print(f"{C_RED}Missing:{C_END} {not_found_count}")

    if not_found_count > 0:
        print(f"\n{C_RED}Warning: Some offsets could not be verified. Please check Offsets.hpp manually.{C_END}")
        sys.exit(1)
    else:
        print(f"\n{C_GREEN}All offsets are fully verified and up to date!{C_END}")

if __name__ == "__main__":
    main()
