#!/usr/bin/env python3
"""
Evitania Update Doctor & Self-Healing Engine.

Doctor (read-only diagnostics, `scripts/update --doctor`):
  - Environment: Python deps, build toolchain.
  - Game files: GameAssembly.dll, global-metadata.dat (encryption check), Steam build drift.
  - Signature DB: config.json integrity, live scan of every signature (no headers written).
  - dump.cs freshness: cross-checks RVAs of matched signatures against dump.cs.
  - Target drift: build_sig_db targets that vanished from dump.cs (+ rename suggestions).
  - C++ consistency: Signatures::/Offsets:: references in src/ vs generated headers.

Self-heal (used by updater.py when a scan fails):
  1. Snapshot config.json + generated headers (automatic rollback on failure).
  2. Live RAM dump if the game is running (encrypted/packed disk binary).
  3. Regenerate dump.cs with Il2CppDumper when stale and the dumper is available.
  4. Rebuild config.json from dump.cs (build_sig_db.py) and re-scan.
  5. Classify remaining failures (removed method / stale dump / pattern drift) with fixes.
"""

import difflib
import importlib.util
import json
import os
import platform
import re
import shutil
import struct
import subprocess
import sys
import time

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.abspath(os.path.join(SCRIPT_DIR, "../.."))
sys.path.insert(0, SCRIPT_DIR)

CONFIG_PATH = os.path.join(PROJECT_ROOT, "config.json")
DUMPED_DIR = os.path.join(
    PROJECT_ROOT, "resources", "dumped"
)  # Git-Ignored, u need dump by urself and put the dump.cs file inside the resources/dumped/
DUMP_CS = os.path.join(DUMPED_DIR, "dump.cs")
SIG_HPP = os.path.join(PROJECT_ROOT, "src", "Modules", "Hooks", "Signatures.hpp")
OFF_HPP = os.path.join(PROJECT_ROOT, "src", "Modules", "Hooks", "Offsets.hpp")
SNAPSHOT_DIR = os.path.join(PROJECT_ROOT, "build", ".update_snapshot")
SRC_DIR = os.path.join(PROJECT_ROOT, "src")
VENDOR_RE = re.compile(r"src[\\/]Modules[\\/](ImGui|MinHook|Il2CppResolver)[\\/]")
METADATA_MAGIC = 0xFAB11BAF

OK, INFO, WARN, FAIL = "OK", "INFO", "WARN", "FAIL"
_ICON = {OK: "[+]", INFO: "[*]", WARN: "[!]", FAIL: "[-]"}


class Report:
    def __init__(self):
        self.findings = []  # (level, area, message, fix)

    def add(self, level, area, message, fix=None):
        self.findings.append((level, area, message, fix))
        line = f"{_ICON[level]} {area:<12} {message}"
        print(line)
        if fix:
            print(f"    -> fix: {fix}")

    def count(self, level):
        return sum(1 for f in self.findings if f[0] == level)

    def to_dict(self):
        return [
            {"level": l, "area": a, "message": m, "fix": f}
            for l, a, m, f in self.findings
        ]


# --------------------------------------------------------------------------- #
# Helpers
# --------------------------------------------------------------------------- #
def game_dir_from_dll(dll_path):
    return os.path.dirname(os.path.abspath(dll_path)) if dll_path else None


def find_metadata(dll_path):
    gdir = game_dir_from_dll(dll_path)
    if not gdir or not os.path.isdir(gdir):
        return None
    for entry in os.listdir(gdir):
        if entry.endswith("_Data"):
            p = os.path.join(
                gdir, entry, "il2cpp_data", "Metadata", "global-metadata.dat"
            )
            if os.path.exists(p):
                return p
    return None


def metadata_is_plain(path):
    try:
        with open(path, "rb") as f:
            magic, version = struct.unpack("<II", f.read(8))
        return magic == METADATA_MAGIC, version
    except Exception:
        return False, None


def find_il2cppdumper():
    """Locate Il2CppDumper via $IL2CPPDUMPER, resources/Tools/Il2CppDumper/, or PATH."""
    candidates = []
    env = os.environ.get("IL2CPPDUMPER")
    if env:
        candidates.append(env)
    base = os.path.join(SCRIPT_DIR, "Il2CppDumper")
    for name in ("Il2CppDumper", "Il2CppDumper.dll", "Il2CppDumper.exe"):
        candidates.append(os.path.join(base, name))
    for name in ("Il2CppDumper", "il2cppdumper"):
        w = shutil.which(name)
        if w:
            candidates.append(w)
    for c in candidates:
        if c and os.path.isfile(c):
            return c
    return None


def dumper_command(dumper):
    if dumper.lower().endswith(".dll"):
        return ["dotnet", dumper]
    if dumper.lower().endswith(".exe") and platform.system() != "Windows":
        return ["wine", dumper]
    return [dumper]


def parse_header_names():
    """Return (signature names, offset qualified names) from generated headers."""
    sigs, offs = set(), set()
    if os.path.exists(SIG_HPP):
        with open(SIG_HPP, encoding="utf-8") as f:
            sigs = set(re.findall(r"constexpr const char\*\s+(\w+)", f.read()))
    if os.path.exists(OFF_HPP):
        stack = []
        with open(OFF_HPP, encoding="utf-8") as f:
            for line in f:
                m = re.match(r"\s*namespace\s+(\w+)", line)
                if m:
                    stack.append(m.group(1))
                    continue
                if re.match(r"\s*}\s*//\s*namespace", line) and stack:
                    stack.pop()
                    continue
                m = re.match(r"\s*constexpr int\s+([\w<>]+)\s*=", line)
                if m:
                    offs.add("::".join(stack[1:] + [m.group(1)]))
    return sigs, offs


def scan_cpp_references():
    """Map Signatures::X / Offsets::Y references in project sources -> [file:line]."""
    sig_refs, off_refs = {}, {}
    sig_re = re.compile(r"\bSignatures::(\w+)")
    off_re = re.compile(r"\bOffsets::((?:\w+::)*\w+)")
    for root, _, files in os.walk(SRC_DIR):
        for fn in files:
            if not fn.endswith((".cpp", ".hpp", ".h")):
                continue
            path = os.path.join(root, fn)
            if VENDOR_RE.search(path) or path in (SIG_HPP, OFF_HPP):
                continue
            rel = os.path.relpath(path, PROJECT_ROOT)
            with open(path, encoding="utf-8", errors="ignore") as f:
                for n, line in enumerate(f, 1):
                    for m in sig_re.finditer(line):
                        sig_refs.setdefault(m.group(1), []).append(f"{rel}:{n}")
                    for m in off_re.finditer(line):
                        off_refs.setdefault(m.group(1), []).append(f"{rel}:{n}")
    return sig_refs, off_refs


def load_targets():
    import build_sig_db

    return build_sig_db


def target_index(bsd):
    """method_key -> (class, method, overload)"""
    idx = {}
    for cls, method, ov, _ in bsd.TARGETS:
        idx[bsd.method_key_for(cls, method, ov)] = (cls, method, ov)
    return idx


def suggest_renames(class_data, cls, method):
    if cls not in class_data:
        close = difflib.get_close_matches(cls, class_data.keys(), n=3, cutoff=0.75)
        return [f"class {c}" for c in close]
    close = difflib.get_close_matches(
        method, class_data[cls]["methods"].keys(), n=3, cutoff=0.5
    )
    return [f"{cls}::{m}" for m in close]


# --------------------------------------------------------------------------- #
# Doctor
# --------------------------------------------------------------------------- #
def check_environment(rep):
    for mod in ("pefile", "capstone"):
        if importlib.util.find_spec(mod):
            rep.add(OK, "python", f"module '{mod}' available")
        else:
            rep.add(FAIL, "python", f"module '{mod}' missing", f"pip install {mod}")
    tools = ["cmake"]
    if platform.system() != "Windows":
        tools.append("x86_64-w64-mingw32-g++")
    for t in tools:
        if shutil.which(t):
            rep.add(OK, "toolchain", f"{t} found")
        else:
            rep.add(FAIL, "toolchain", f"{t} not found in PATH", f"install {t}")
    dumper = find_il2cppdumper()
    if dumper:
        rep.add(OK, "toolchain", f"Il2CppDumper: {dumper}")
    else:
        rep.add(
            INFO,
            "toolchain",
            "Il2CppDumper not found (auto dump.cs regeneration disabled)",
            "set $IL2CPPDUMPER or place it in resources/Tools/Il2CppDumper/",
        )


def check_game(rep, dll_path, config):
    if not dll_path or not os.path.exists(dll_path):
        rep.add(
            FAIL,
            "game",
            f"GameAssembly.dll not found ({dll_path})",
            "pass --dll <path>",
        )
        return
    rep.add(OK, "game", f"GameAssembly.dll: {dll_path}")
    meta = find_metadata(dll_path)
    if not meta:
        rep.add(WARN, "game", "global-metadata.dat not found next to GameAssembly.dll")
    else:
        plain, ver = metadata_is_plain(meta)
        if plain:
            rep.add(OK, "game", f"global-metadata.dat is plain (v{ver})")
        else:
            rep.add(
                FAIL,
                "game",
                "global-metadata.dat is encrypted/obfuscated (bad magic)",
                "launch the game and run `scripts/update --from-mem`",
            )
    try:
        from updater import find_steam_manifest, read_manifest_info

        info = read_manifest_info(find_steam_manifest(dll_path))
        rec = config.get("steam_build_id") if config else None
        if info["buildid"] != "unknown" and info["buildid"] != rec:
            rep.add(
                WARN,
                "game",
                f"Steam build {info['buildid']} != recorded {rec}",
                "scripts/update",
            )
        else:
            rep.add(OK, "game", f"Steam build {info['buildid']} matches config.json")
    except Exception as e:
        rep.add(INFO, "game", f"Steam manifest check skipped ({e})")


def check_config(rep):
    if not os.path.exists(CONFIG_PATH):
        rep.add(
            FAIL,
            "config",
            "config.json missing",
            "python3 resources/Tools/build_sig_db.py",
        )
        return None
    try:
        with open(CONFIG_PATH, encoding="utf-8") as f:
            config = json.load(f)
    except json.JSONDecodeError as e:
        rep.add(
            FAIL,
            "config",
            f"config.json is invalid JSON: {e}",
            "git checkout config.json",
        )
        return None
    sigs = config.get("signatures", {})
    empty = [
        k
        for k, v in sigs.items()
        if not k.startswith("Il2Cpp_") and not v.get("signature")
    ]
    rep.add(OK, "config", f"config.json valid ({len(sigs)} entries)")
    if empty:
        rep.add(WARN, "config", f"entries with empty signature: {', '.join(empty)}")
    return config


def check_scan_and_dump(rep, dll_path, config):
    """Live scan + dump.cs cross-check + target drift. Returns analysis dict."""
    analysis = {
        "scan": None,
        "dump_stale": None,
        "missing_targets": [],
        "class_data": None,
    }
    if not config or not dll_path or not os.path.exists(dll_path):
        return analysis
    from aobgenerator import generate_signatures_and_offsets

    scan = generate_signatures_and_offsets(
        dll_path=dll_path, sig_db_path=CONFIG_PATH, verbose=False, write=False
    )
    analysis["scan"] = scan
    total = scan["total_signatures"]
    if scan["success"]:
        rep.add(
            OK,
            "scan",
            f"{scan['matched_signatures']}/{total} signatures, all offsets resolved",
        )
    else:
        for k in scan["broken_signatures"]:
            rep.add(FAIL, "scan", f"broken signature: {k}")
        for k, n in scan["ambiguous_signatures"]:
            rep.add(FAIL, "scan", f"ambiguous signature: {k} ({n} matches)")
        for k in scan["failed_offsets"]:
            rep.add(FAIL, "scan", f"offset extraction failed: {k}")
    for k in scan["semantic_shifts"]:
        rep.add(INFO, "scan", f"semantic shift auto-resolved: {k}")

    if not os.path.exists(DUMP_CS):
        rep.add(
            FAIL,
            "dump.cs",
            "resources/dumped/dump.cs missing",
            "run Il2CppDumper (see update-workflow)",
        )
        return analysis

    bsd = load_targets()
    class_data = bsd.get_offsets(DUMP_CS)
    analysis["class_data"] = class_data
    tidx = target_index(bsd)

    # RVA cross-check: matched signature RVA must equal dump.cs RVA.
    checked, mismatched = 0, []
    for key, rva in scan["matched_rvas"].items():
        if key not in tidx:
            continue
        cls, method, ov = tidx[key]
        rvas = class_data.get(cls, {}).get("methods", {}).get(method, [])
        if ov < len(rvas):
            checked += 1
            if rvas[ov] != rva:
                mismatched.append(key)
    if checked:
        stale = len(mismatched) > checked // 2
        analysis["dump_stale"] = stale
        if stale:
            rep.add(
                FAIL,
                "dump.cs",
                f"STALE: {len(mismatched)}/{checked} RVAs disagree with GameAssembly.dll",
                "re-dump with Il2CppDumper (or set $IL2CPPDUMPER and let self-heal do it)",
            )
        elif mismatched:
            rep.add(
                WARN,
                "dump.cs",
                f"{len(mismatched)}/{checked} RVA mismatches: {', '.join(mismatched)}",
            )
        else:
            rep.add(
                OK,
                "dump.cs",
                f"in sync with GameAssembly.dll ({checked} RVAs verified)",
            )
    elif os.path.getmtime(DUMP_CS) < os.path.getmtime(dll_path):
        analysis["dump_stale"] = True
        rep.add(WARN, "dump.cs", "older than GameAssembly.dll (no RVAs to verify)")

    # Target drift: methods that no longer exist in the game.
    # Severity: FAIL if C++ still hooks it (build will break), WARN if dead target.
    sig_refs, _ = scan_cpp_references()
    for key, (cls, method, ov) in tidx.items():
        rvas = class_data.get(cls, {}).get("methods", {}).get(method, [])
        if ov >= len(rvas):
            analysis["missing_targets"].append(key)
            sugg = suggest_renames(class_data, cls, method)
            used = sig_refs.get(key)
            rep.add(
                FAIL if used and not analysis["dump_stale"] else WARN,
                "targets",
                f"{cls}::{method}[{ov}] not in dump.cs (removed/renamed?)"
                + (f" - used at {', '.join(used[:2])}" if used else " - unused by C++")
                + (f" - similar: {', '.join(sugg)}" if sugg else ""),
                f"update/remove the entry in build_sig_db.py TARGETS"
                + (f" and its hook ({key})" if used else ""),
            )
    if not analysis["missing_targets"]:
        rep.add(OK, "targets", f"all {len(tidx)} build_sig_db targets exist in dump.cs")
    return analysis


def check_cpp(rep, analysis=None):
    hdr_sigs, hdr_offs = parse_header_names()
    sig_refs, off_refs = scan_cpp_references()
    bad = 0
    for name, locs in sorted(sig_refs.items()):
        if name not in hdr_sigs:
            bad += 1
            removed = analysis and name in analysis.get("missing_targets", [])
            rep.add(
                FAIL,
                "c++",
                f"Signatures::{name} undefined (used at {', '.join(locs[:3])})"
                + (" - method removed from game" if removed else ""),
                (
                    "drop the hook"
                    if removed
                    else "add target to build_sig_db.py and rebuild DB"
                ),
            )
    for name, locs in sorted(off_refs.items()):
        if name not in hdr_offs:
            bad += 1
            rep.add(
                FAIL,
                "c++",
                f"Offsets::{name} undefined (used at {', '.join(locs[:3])})",
                "add an extraction rule in build_sig_db.py TARGETS",
            )
    unused = sorted(hdr_sigs - set(sig_refs))
    if unused:
        rep.add(
            INFO,
            "c++",
            f"{len(unused)} signature(s) generated but unused: {', '.join(unused)}",
        )
    if not bad:
        rep.add(
            OK,
            "c++",
            f"{len(sig_refs)} signature + {len(off_refs)} offset references resolve",
        )
    return bad


def run_doctor(dll_path=None, report_path=None):
    from aobgenerator import find_game_assembly

    print("\n" + "=" * 65)
    print("  " + "EVITANIA UPDATE DOCTOR".center(61))
    print("=" * 65)
    rep = Report()
    dll_path = dll_path or find_game_assembly()
    check_environment(rep)
    config = check_config(rep)
    check_game(rep, dll_path, config)
    analysis = check_scan_and_dump(rep, dll_path, config)
    check_cpp(rep, analysis)

    fails, warns = rep.count(FAIL), rep.count(WARN)
    print("=" * 65)
    verdict = "HEALTHY" if not fails else "UNHEALTHY"
    print(f"  Verdict: {verdict}  ({fails} failure(s), {warns} warning(s))")
    if fails:
        print("  Try: scripts/update --force   (self-heal is on by default)")
    print("=" * 65)
    if report_path:
        with open(report_path, "w") as f:
            json.dump(rep.to_dict(), f, indent=2)
        print(f"[+] Saved doctor report -> {report_path}")
    return 0 if not fails else 1


# --------------------------------------------------------------------------- #
# Self-heal
# --------------------------------------------------------------------------- #
SNAPSHOT_FILES = [CONFIG_PATH, SIG_HPP, OFF_HPP]


def snapshot():
    os.makedirs(SNAPSHOT_DIR, exist_ok=True)
    for p in SNAPSHOT_FILES:
        if os.path.exists(p):
            shutil.copy2(p, os.path.join(SNAPSHOT_DIR, os.path.basename(p)))
    print(f"[*] Snapshot saved -> {os.path.relpath(SNAPSHOT_DIR, PROJECT_ROOT)}")


def rollback():
    restored = 0
    for p in SNAPSHOT_FILES:
        s = os.path.join(SNAPSHOT_DIR, os.path.basename(p))
        if os.path.exists(s):
            shutil.copy2(s, p)
            restored += 1
    print(f"[*] Rolled back {restored} file(s) from snapshot (config.json + headers).")


def regenerate_dump_cs(dll_path):
    dumper = find_il2cppdumper()
    meta = find_metadata(dll_path)
    if not dumper or not meta:
        print(
            "[!] Cannot regenerate dump.cs automatically (Il2CppDumper or metadata missing)."
        )
        return False
    plain, _ = metadata_is_plain(meta)
    if not plain:
        print("[!] global-metadata.dat is encrypted; Il2CppDumper cannot process it.")
        return False
    out_dir = os.path.join(PROJECT_ROOT, "build", ".il2cppdumper_out")
    shutil.rmtree(out_dir, ignore_errors=True)
    os.makedirs(out_dir, exist_ok=True)
    cmd = dumper_command(dumper) + [dll_path, meta, out_dir]
    print(f"[*] Regenerating dump.cs: {' '.join(cmd)}")
    try:
        subprocess.run(cmd, stdin=subprocess.DEVNULL, timeout=900, cwd=out_dir)
    except Exception as e:
        print(f"[-] Il2CppDumper failed: {e}")
        return False
    new_dump = os.path.join(out_dir, "dump.cs")
    if not os.path.exists(new_dump):
        print("[-] Il2CppDumper produced no dump.cs")
        return False
    os.makedirs(DUMPED_DIR, exist_ok=True)
    if os.path.exists(DUMP_CS):
        shutil.copy2(DUMP_CS, DUMP_CS + ".bak")
    shutil.move(new_dump, DUMP_CS)
    for extra in ("script.json", "il2cpp.h"):
        p = os.path.join(out_dir, extra)
        if os.path.exists(p):
            shutil.move(p, os.path.join(DUMPED_DIR, extra))
    print("[+] dump.cs regenerated (previous kept as dump.cs.bak)")
    return True


def rebuild_sig_db(dll_path):
    print("[*] Rebuilding config.json from dump.cs (build_sig_db.py)...")
    proc = subprocess.run(
        [
            sys.executable,
            os.path.join(SCRIPT_DIR, "build_sig_db.py"),
            "--dll",
            dll_path,
        ],
        cwd=PROJECT_ROOT,
        capture_output=True,
        text=True,
    )
    for line in proc.stdout.splitlines():
        if not line.startswith("[+] Added") and "Found GameAssembly" not in line:
            print("    " + line)
    if proc.returncode != 0:
        print(proc.stderr)
        return False
    return True


def heal(dll_path, scan_fn):
    """Attempt self-healing. scan_fn(dll) -> results. Returns (results, dll_path, log)."""
    log = []
    from aobgenerator import generate_signatures_and_offsets  # noqa: F401

    # Stage 1: dump.cs freshness -> regenerate if stale & possible.
    stale = None
    try:
        bsd = load_targets()
        cd = bsd.get_offsets(DUMP_CS) if os.path.exists(DUMP_CS) else None
        if cd is None:
            stale = True
        elif os.path.getmtime(DUMP_CS) < os.path.getmtime(dll_path):
            stale = True
    except Exception:
        stale = None
    if stale:
        print(
            "\n[HEAL 1/3] dump.cs is older than GameAssembly.dll -> attempting regeneration"
        )
        if regenerate_dump_cs(dll_path):
            log.append("regenerated dump.cs")
        else:
            log.append("dump.cs possibly stale (could not regenerate)")
    else:
        print("\n[HEAL 1/3] dump.cs is newer than GameAssembly.dll -> using as-is")

    # Stage 2: rebuild signature DB from dump.cs.
    print("\n[HEAL 2/3] Regenerating signatures from dump.cs")
    if not rebuild_sig_db(dll_path):
        log.append("build_sig_db failed")
        return None, dll_path, log
    log.append("rebuilt config.json")

    # Stage 3: rescan.
    print("\n[HEAL 3/3] Re-scanning GameAssembly.dll")
    results = scan_fn(dll_path)
    if results["success"]:
        log.append("rescan succeeded")
    return results, dll_path, log


BUILD_ERR_RE = re.compile(r"'(\w+)'\s*:?\s*is not a member of '(Signatures|Offsets)'")


def diagnose_build_output(output):
    """Turn compiler errors about generated headers into actionable advice."""
    hits = sorted(set(BUILD_ERR_RE.findall(output)))
    if not hits:
        return False
    print("\n[DOCTOR] Build failed on generated-header symbols:")
    try:
        bsd = load_targets()
        cd = bsd.get_offsets(DUMP_CS)
        tidx = target_index(bsd)
    except Exception:
        cd, tidx = None, {}
    sig_refs, off_refs = scan_cpp_references()
    for name, ns in hits:
        locs = (sig_refs if ns == "Signatures" else off_refs).get(name, [])
        where = f" (used at {', '.join(locs[:3])})" if locs else ""
        cause = ""
        if ns == "Signatures" and name in tidx and cd is not None:
            cls, method, ov = tidx[name]
            if ov >= len(cd.get(cls, {}).get("methods", {}).get(method, [])):
                sugg = suggest_renames(cd, cls, method)
                cause = " -> method REMOVED from game" + (
                    f"; similar: {', '.join(sugg)}" if sugg else ""
                )
        elif ns == "Signatures" and name not in tidx:
            cause = " -> no build_sig_db target defines it"
        print(f"    - {ns}::{name}{where}{cause}")
    print("    Fix: remove/redirect those hooks, or re-run with --allow-degraded.")
    return True


if __name__ == "__main__":
    import argparse

    ap = argparse.ArgumentParser(description="Evitania update doctor")
    ap.add_argument("--dll", default=None)
    ap.add_argument("--report", default=None, help="write JSON report to path")
    a = ap.parse_args()
    sys.exit(run_doctor(a.dll, a.report))
