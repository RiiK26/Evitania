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
- **UI/Feature Sync**: The `src/Features/<Domain>/` directory structure MUST map exactly 1:1 with the ImGui Tab names defined in `Menu.cpp` (e.g., `Combat`, `Player`, `Economy`, `Curio`, `Engineer`, `Hunter`). If a feature is displayed in the "Player" tab, its source code MUST reside in `src/Features/Player/`.
- **Hooking**: Always prefer `HOOK_SIGNATURE` (AOB pattern scanning) or `HOOK_METHOD` over static `HOOK_OFFSET` where possible. Dynamic resolving and signatures are resilient to game updates and obfuscation, whereas hardcoded offsets break immediately.
- **Offset Extraction**: NEVER use `IL2CPP::Class::Utils::GetFieldOffset` in C++ for field offsets, as it breaks when games are obfuscated. Instead, rely on `aobgenerator.py` which dynamically extracts offsets via Capstone machine-code analysis. Use the generated `Offsets.hpp` variables (e.g. `Offsets::alive`).
- **Signature Database**: `config.json` at the root of the project serves as the baseline signature and extraction rule database.
- **Toggles & Config**: Store feature toggles and parameters (like damage values) in `Menu::ConfigData`. Load/save these values using `config.txt` at the project binaries directory. Avoid hardcoding magic numbers for features.
- **Initialization**: Every feature file must have an `Initialize()` and `Uninitialize()` function, which is registered in `src/Modules/Hooks/Hooks.cpp`.
- **IDE Parsing (`clangd`)**: The project uses a `.clangd` file to force-include `IL2CPP_Resolver.hpp` for internal module headers. Do NOT manually add `#include "../../IL2CPP_Resolver.hpp"` to internal headers, as it causes circular dependency "incomplete type" errors in the IDE.

## Boundaries
- Do not dump game assemblies into the Git tree unless they are strictly inside `resources/dumped/`.
- Both `dump.cs` and `GameAssembly.dll` are required for maintaining updates. `build_sig_db.py` relies on `dump.cs` to generate the signature database (`config.json`), which `aobgenerator.py` then uses against `GameAssembly.dll`.
- Do not commit large compiled binaries (`.dll`, `.exe`) to version control.
- Avoid using `.hpp` to define logic; use `.hpp` for declarations and `.cpp` for implementations.

## Branch Workflow

The project uses a strict 2-branch strategy for stable releases (that contain anti-RE protections) and clean builds (that do not contain anti-RE protections):

- **`main`**: The default branch for stable releases. Contains advanced anti-reverse engineering protections
- **`clean`**: Used for producing clean, unobfuscated builds that include PDB debug symbols. Contains no anti-RE protections

## Anti-RE & VMProtect Configuration

- **C++ Anti-RE**: The `main` branch includes basic, lightweight Anti-RE measures (e.g., `skCrypt` for strings, `CheckDebugger`, `ErasePEHeaders` in `src/Cores/AntiRE.cpp`). These are fast and should not impact performance.
- **VMProtect Rules (`resources/template/Evitania.dll.vmp`)**:
  - **`CompilationType="2"` (Virtualization)**: Use ONLY for one-time initialization functions (e.g., `DllMain`, `Hooks::Initialize`, `Menu::Initialize`). Never use this for recurring functions, as it severely degrades performance (up to 100x slower).
  - **`CompilationType="1"` (Mutation)**: Use for event-driven cheat hooks (e.g., `TakeDamage`, `AddExperience`, `InitiatePurchase`). Provides adequate protection with minimal overhead.
  - **`CompilationType="0"` (None)**: Use explicitly for all real-time/per-frame functions (e.g., `Menu::hkPresent`, `Menu::WndProc`, `Hook_MovementControl_Move`). Protecting these functions will cause massive CPU spikes and stuttering. Do NOT protect them.

## Patterns

### Feature Implementation Pattern
Every feature should be modular and follow this structure (e.g. `src/Features/Combat/SpeedHack.cpp`). It defines the original function pointer, the hook function, and registers it in `Initialize()`:

```cpp
#include "SpeedHack.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace SpeedHack
  {
    // 1. Declare original function pointer
    void (*Orig_Time_set_timeScale)(float value, void* method_info);

    // 2. Define the hook overriding the behavior
    void Hook_Time_set_timeScale(float value, void* method_info)
    {
      if (Menu::Config.bSpeedHack) {
        value = Menu::Config.fSpeedMultiplier;
      }
      Orig_Time_set_timeScale(value, method_info);
    }

    // 3. Register the hook in Initialize()
    void Initialize()
    {
      HOOK_SIGNATURE(
        "Time::set_timeScale", Signatures::Time_set_timeScale, Hook_Time_set_timeScale, Orig_Time_set_timeScale
      );
    }

    void Uninitialize() { }
  }  // namespace SpeedHack
}  // namespace Features
```
