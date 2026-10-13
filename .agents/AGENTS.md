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
- **Find Field Usage & Disassemble**: `python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> [--disasm]`
- **Update Workflow Guide**: Reference [.agents/workflows/update-workflow.md](file:///home/riik/Projects/Evitania/.agents/workflows/update-workflow.md) for full update procedure.

## Code Conventions
- **Modular Architecture**: 1 Feature = 1 `.hpp`/`.cpp` pair located inside `src/Features/<Domain>/` (e.g. `src/Features/Combat/GodMode.cpp`). **No god files.**
- **UI/Feature Sync**: The `src/Features/<Domain>/` directory structure MUST map exactly 1:1 with the ImGui Tab names defined in `Menu.cpp` (e.g., `Combat`, `Player`, `Economy`, `Curio`, `Engineer`, `Hunter`, `Timeline`). If a feature is displayed in the "Player" tab, its source code MUST reside in `src/Features/Player/`.
- **Hooking**: Always prefer `HOOK_SIGNATURE` (AOB pattern scanning) or `HOOK_METHOD` over static `HOOK_OFFSET` where possible. Dynamic resolving and signatures are resilient to game updates and obfuscation, whereas hardcoded offsets break immediately.
- **Offset Extraction & Centralization**: NEVER use `IL2CPP::Class::Utils::GetFieldOffset` in C++ for field offsets, as it breaks when games are obfuscated. Rely on `aobgenerator.py` which dynamically extracts offsets via Capstone machine-code analysis. Centralize all offsets in `Offsets.hpp`: low-level runtime structures are separated under `Offsets::Il2Cpp::...` (e.g., `Array::data`, `Dictionary::entryStride`), while gameplay field offsets reside under `Offsets::...` (e.g. `Offsets::alive`). Never hardcode magic numbers for structs or collection layouts in hooks or helpers.
- **Signature Database**: `config.json` at the root of the project serves as the baseline signature and extraction rule database. Generic runtime collections are categorized as `Il2Cpp_<Struct>` (e.g., `Il2Cpp_Array`, `Il2Cpp_Dictionary`).
- **Toggles & Config**: Store feature toggles and parameters (like damage values) in `Menu::ConfigData`. Load/save these values using `config.txt` at the project binaries directory. Avoid hardcoding magic numbers for features.
- **Initialization**: Every feature file must have an `Initialize()` and `Uninitialize()` function, which is registered in `src/Modules/Hooks/Hooks.cpp`.
- **Performance**: Game loops and property getters (e.g., `get_Cost`) can be called thousands of times per second (especially during loading screens). **NEVER** use expensive operations like `std::unordered_map` lookups or heap allocations directly in the hot-path of these hooks, as it will cripple boot and framerate times. Cache or aggressively bypass these checks whenever possible.
- **IDE Parsing (`clangd`)**: The project uses a `.clangd` file to force-include `IL2CPP_Resolver.hpp` for internal module headers. Do NOT manually add `#include "../../IL2CPP_Resolver.hpp"` to internal headers, as it causes circular dependency "incomplete type" errors in the IDE.

## Boundaries
- Do not dump game assemblies into the Git tree unless they are strictly inside `resources/dumped/`.
- Both `dump.cs` and `GameAssembly.dll` are required for maintaining updates. `build_sig_db.py` relies on `dump.cs` to generate the signature database (`config.json`), which `aobgenerator.py` then uses against `GameAssembly.dll`.
- Do not commit large compiled binaries (`.dll`, `.exe`) to version control.
- Avoid using `.hpp` to define logic; use `.hpp` for declarations and `.cpp` for implementations.

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

### Gotchas / Known Quirks
- **Property Setters vs Array Removals**: When trying to hook item consumption (e.g. Infinite Items), intercepting the property setter (like `ItemDetail::set_Amount`) is often **insufficient**. The game logic often completely destroys/removes the object from the inventory array for consumables or full-stack uses by calling higher-level manager methods like `BaseStorageService::Remove` or `BaseStorageService::TryRemove`. Hook these root array managers to properly prevent removals.
- **Backing Fields in Regex**: In `build_sig_db.py` and `aobgenerator.py`, we extract field offsets via Capstone. Many unity fields are compiler-generated backing fields containing angle brackets (e.g., `<Amount>k__BackingField`). Our Python scripts have been updated to support `[a-zA-Z0-9_<>]+` in regex to correctly parse them. Do NOT regress this regex support if you edit the scripts.
- **Centralizing Offsets**: Never hardcode hex offsets in hooks (like `0x18`). ALWAYS extract them dynamically via `build_sig_db.py`'s extraction rules and place them in `Offsets.hpp`.
- **UI Visuals vs. Spending Validation (Dual-Mode Hooking)**:
  - Game UI methods (e.g. `EngineerShopDescription::UpdateVisuals`) enumerate price dictionaries (`Dictionary<ISpendable, float>`) to render currency icons. If `_count == 0`, icons vanish.
  - However, game spend resolvers (e.g. `SpendResolver`) enforce `if (amount <= 0) return false;`, blocking zero-cost purchases.
  - Solution: Use a `thread_local bool` flag during the purchase method (e.g. `EngineerSystemUI::TryBuyUpgrade`) to provide an empty dictionary during the transaction (bypassing spend checks), while providing zeroed float values (`Utils::Il2Cpp::ZeroDictionaryFloatValues`) during UI rendering so icons remain visible with `0` cost.
- **Feature Isolation**: Never reuse general store toggles (`bFreeStore`) across in-game skill systems (Combat, Woodcutting, Mining talent points, or Bonfire upgrades). Keep store purchases (`SteamPurchaseService` / `MobilePurchaseService`) strictly segregated from progression mechanics.

## Game Acknowledge
- **Trade & Economy Architecture**:
  - `ISpendable` / `IAcquirable`: Base interfaces implemented by `TradableBase` (`Currency`, `Item`, `EngineerItem`, etc.).
  - `TradeUtils.Trader`: Central router for transactions (`Trader.CanSpend`, `Trader.Spend`, `Trader.Add`).
  - Stockpile Spending: `EngineerItem` delegates spending to `EngineerService.SpendResolver`.
- **IL2CPP Dictionary Layout**:
  - `_entries` array at `Offsets::entries` (`0x18`), `_count` at `Offsets::count` (`0x20`).
  - Elements start at `+0x20` inside `_entries`. Each entry is 24 bytes (`0x18`): `hashCode` (int32, +0), `next` (int32, +4), `key` (pointer, +8), `value` (float, +16).
- **Stage & Map Identifiers**:
  - In `StageHopper`, Act 1 stages use numeric formatting (e.g. `1.6`), whereas boss stages and higher acts format stage identifiers as strings (e.g. `1.boss-2`). Always handle both numeric and string representations when resolving player location.
- **Boot Flow & Hot-Path Hooks**:
  - Game attaches and initializes services sequentially (`AntiCheatService`, `PositionerService`, `MapUi`, `EngineerService`).
  - Hooks running during boot or in getters called by game loops must avoid expensive string operations, unbounded pattern searches, or dynamic memory allocations to prevent boot/login screen freezes.
