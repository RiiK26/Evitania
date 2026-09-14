# Project: Evitania

## Tech Stack
- C++20
- MinHook (Hooking)
- ImGui (Menu/UI)
- Il2CppResolver (Dynamic Unity Method/Class Resolution)
- Capstone Engine (Static Assembly Analysis & Offset Extraction)
- CMake, MinGW-w64 (Build System)

## Commands
- **Build Project**: `scripts/build`
- **Run the Project**: `scripts/run`
- **Generate Signatures & Offsets**: `python3 resources/Tools/aobgenerator.py`
- **Build Baseline Signature DB**: `python3 resources/Tools/build_sig_db.py`

## Code Conventions
- **Modular Architecture**: 1 Feature = 1 `.hpp`/`.cpp` pair located inside `src/Features/<Domain>/` (e.g. `src/Features/Combat/GodMode.cpp`). **No god files.**
- **Hooking**: Always prefer `HOOK_SIGNATURE` (AOB pattern scanning) or `HOOK_METHOD` over static `HOOK_OFFSET` where possible. Dynamic resolving and signatures are resilient to game updates and obfuscation, whereas hardcoded offsets break immediately.
- **Offset Extraction**: NEVER use `IL2CPP::Class::Utils::GetFieldOffset` in C++ for field offsets, as it breaks when games are obfuscated. Instead, rely on `aobgenerator.py` which dynamically extracts offsets via Capstone machine-code analysis. Use the generated `Offsets.hpp` variables (e.g. `Offsets::alive`).
- **Signature Database**: `config.json` at the root of the project serves as the baseline signature and extraction rule database.
- **Toggles & Config**: Store feature toggles and parameters (like damage values) in `Menu::ConfigData`. Load/save these values using `config.txt` at the project binaries directory. Avoid hardcoding magic numbers for features.
- **Initialization**: Every feature file must have an `Initialize()` and `Uninitialize()` function, which is registered in `src/Modules/Hooks/Hooks.cpp`.
- **IDE Parsing (`clangd`)**: The project uses a `.clangd` file to force-include `IL2CPP_Resolver.hpp` for internal module headers. Do NOT manually add `#include "../../IL2CPP_Resolver.hpp"` to internal headers, as it causes circular dependency "incomplete type" errors in the IDE.

## Boundaries
- Do not dump game assemblies into the Git tree unless they are strictly inside `resources/dumped/`.
- `dump.cs` is no longer required for maintaining updates. `GameAssembly.dll` is the only requirement for `aobgenerator.py` to regenerate offsets.
- Do not commit large compiled binaries (`.dll`, `.exe`) to version control.
- Avoid using `.hpp` to define logic; use `.hpp` for declarations and `.cpp` for implementations.
