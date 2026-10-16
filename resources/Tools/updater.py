#!/usr/bin/env python3
"""
Evitania Unified Update Runner
Automates the end-to-end update workflow:
1. Detects Steam updates (build ID & assembly SHA-256)
2. Scans and validates AOB signatures & semantic offsets
3. Re-baselines config.json metadata
4. Invokes the project build script to produce release binaries
"""

import argparse
import hashlib
import json
import os
import platform
import re
import subprocess
import sys
import time

# Add Tools directory to sys.path
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../.."))
sys.path.insert(0, SCRIPT_DIR)

from aobgenerator import find_game_assembly, generate_signatures_and_offsets

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

    proc = subprocess.run(cmd, cwd=PROJECT_ROOT)
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

    args = parser.parse_args()

    print_banner("EVITANIA AUTOMATED UPDATE RUNNER")

    # 1. Locate Game Assembly & Steam Manifest
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

    # 2. Check config.json Metadata
    if not os.path.exists(CONFIG_PATH):
        print(f"[-] Error: config.json not found at {CONFIG_PATH}")
        sys.exit(1)

    with open(CONFIG_PATH, "r", encoding="utf-8") as f:
        config = json.load(f)

    recorded_build_id = config.get("steam_build_id")
    recorded_sha256 = config.get("assembly_sha256")
    game_version = config.get("version", "unknown")

    is_updated = (
        recorded_build_id != current_build_id or recorded_sha256 != current_sha256
    )

    if args.check:
        if is_updated:
            print(
                f"\n[!] UPDATE AVAILABLE: Current Steam Build {current_build_id} != Recorded {recorded_build_id}"
            )
            sys.exit(1)
        else:
            print(f"\n[+] Game is UP TO DATE (Build {current_build_id}).")
            sys.exit(0)

    if not is_updated and not args.force:
        print(f"\n[*] Game binary is already up to date:")
        print(f"    - Steam Build ID:  {current_build_id} (Matches recorded)")
        print(f"    - Assembly SHA256: {current_sha256[:16]}... (Matches recorded)")
        print(
            "\n[*] No update necessary. Use '--force' or '-f' to re-scan signatures and rebuild anyway."
        )
        sys.exit(0)

    if is_updated:
        print(f"\n[!] Game update detected!")
        print(f"    - Steam Build:    {recorded_build_id} -> {current_build_id}")
        print(
            f"    - Assembly Hash:  {str(recorded_sha256)[:16]}... -> {current_sha256[:16]}..."
        )
    else:
        print(f"\n[*] Running forced update scan and rebuild...")

    # 3. Execute Signature Scanning & Semantic Offset Extraction
    start_time = time.time()
    results = generate_signatures_and_offsets(
        dll_path=dll_path, sig_db_path=CONFIG_PATH, verbose=True
    )
    elapsed = time.time() - start_time

    # 4. Validation Gate
    if not results["success"]:
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

        print("\n[*] Recommended Actions:")
        print(
            "    1. Run `python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> --disasm` to check changes."
        )
        print(
            "    2. If the game is obfuscated, run the Deobfuscator Bridge to correlate RVAs."
        )
        sys.exit(1)

    # 5. Update config.json metadata
    ordered_config = {
        "project_name": config.get("project_name", "Evitania"),
        "version": config.get("version", game_version),
        "steam_build_id": current_build_id,
        "assembly_sha256": current_sha256,
        "last_updated_timestamp": int(time.time()),
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
            print("[-] Compilation failed. Check the build logs above.")
            sys.exit(1)

    # 7. Summary Report
    print_banner("UPDATE & EXTRACTION SUMMARY")
    print(f"  Game Version:        Evitania v{game_version}")
    print(f"  Steam Build ID:      {current_build_id}")
    print(f"  Assembly SHA-256:    {current_sha256[:20]}...")
    print(
        f"  Signatures Scanned:  {results['matched_signatures']} / {results['total_signatures']} (100% Validated)"
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
        print(f"  Status:              READY TO RUN (scripts/run)")
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
