# Project: Evitania

## Tech Stack
- C++20
- MinHook (Hooking)
- ImGui (Menu/UI)
- Il2CppResolver (Dynamic Unity Method/Class Resolution)
- CMake, MinGW-w64 (Build System)

## Commands
- **Windows Build**: `scripts/build.bat`
- **Linux Build**: `scripts/build.sh` (Always uses Release build with optimizations by default)
- **Linux Run & Inject**: `scripts/run.sh`
- **Generate Signatures**: `python3 resources/Tools/aobgenerator.py`

## Code Conventions
- **Modular Architecture**: 1 Feature = 1 `.hpp`/`.cpp` pair located inside `src/Features/<Domain>/` (e.g. `src/Features/Combat/GodMode.cpp`). **No god files.**
- **Hooking**: Always prefer `HOOK_SIGNATURE` (AOB pattern scanning) or `HOOK_METHOD` over static `HOOK_OFFSET` where possible. Dynamic resolving and signatures are resilient to game updates and obfuscation, whereas hardcoded offsets break immediately. Run `aobgenerator.py` to generate robust signatures for `Signatures.hpp`. Use `Offsets.hpp` exclusively for struct/class **fields**, not functions.
- **Toggles & Config**: Store feature toggles and parameters (like damage values) in `Menu::ConfigData`. Load/save these values using `config.txt` at the project root. Avoid hardcoding magic numbers for features.
- **Initialization**: Every feature file must have an `Initialize()` and `Uninitialize()` function, which is registered in `src/Modules/Hooks/Hooks.cpp`.
- **IDE Parsing (`clangd`)**: The project uses a `.clangd` file to force-include `IL2CPP_Resolver.hpp` for internal module headers. Do NOT manually add `#include "../../IL2CPP_Resolver.hpp"` to internal headers, as it causes circular dependency "incomplete type" errors in the IDE.

## Boundaries
- Do not dump game assemblies into the Git tree unless they are strictly inside `resources/dumped/`.
- Do not commit large compiled binaries (`.dll`, `.exe`) to version control.
- Avoid using `.hpp` to define logic; use `.hpp` for declarations and `.cpp` for implementations.
