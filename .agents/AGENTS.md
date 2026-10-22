# Project: Evitania

## Tech Stack
- C++20
- MinHook (Hooking)
- ImGui (Menu/UI)
- Il2CppResolver (Dynamic Unity Method/Class Resolution)
- Capstone Engine (Static Assembly Analysis & Offset Extraction)
- CMake, MinGW-w64 (Build System)

## Commands
- **Unified Game Update & Rebuild**: `scripts/update` (Detects Steam update, validates signatures & semantic offsets, updates config.json, and rebuilds)
- **Check Game Update Status**: `scripts/update --check`
- **Build Project**: `scripts/build [release|clean|debug|no-injector]` (Cross-compiles with MinGW on Linux or builds with MSVC on Windows)
- **Run the Project**: `scripts/run.sh` (Linux Proton / Steam Deck launch option) or `scripts\run.bat` (Windows)
- **Verify Code Formatting**:
  - C/C++: `git ls-files '*.cpp' '*.hpp' '*.h' | grep -vE 'src/Modules/(ImGui|MinHook|Il2CppResolver)/' | xargs clang-format --dry-run --Werror --Wno-error=unknown`
  - Python: `ruff format --check resources/Tools/*.py`
- **Generate Signatures & Offsets**: `python3 resources/Tools/aobgenerator.py`
- **Build Baseline Signature DB**: `python3 resources/Tools/build_sig_db.py`
- **Find Field Usage & Disassemble**: `python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> [--disasm]`
- **Deobfuscator & Symbol Bridge**: `python3 resources/Tools/deobfuscator.py [--all|--ghidra|--ida|--patch-dump]` (Maps obfuscated names to canonical symbols and exports Ghidra/IDA renamers)
- **Live Memory Dumper**: `python3 resources/Tools/dump_memory.py` (Dumps decrypted GameAssembly.dll & metadata from running process RAM for encrypted binary resilience; or run `scripts/update --from-mem`)
- **Update Workflow Guide**: Reference [update workflow](/.agents/workflows/update-workflow.md) for full update procedure.

## Code Conventions
- **Modular Architecture**: 1 Feature = 1 `.hpp`/`.cpp` pair located inside `src/Features/<Domain>/` (e.g. `src/Features/Combat/GodMode.cpp`). **No god files.**
- **UI/Feature Sync**: The `src/Features/<Domain>/` directory structure MUST map exactly 1:1 with the ImGui Tab names defined in `UI.cpp` (e.g., `Combat`, `Player`, `Economy`, `Curio`, `Engineer`, `Hunter`, `Hourglass`, `Bonfire`). If a feature is displayed in the "Hourglass" tab, its source code MUST reside in `src/Features/Hourglass/`.
- **Hooking**: Always prefer `HOOK_SIGNATURE` (AOB pattern scanning) or `HOOK_METHOD` over static `HOOK_OFFSET` where possible. Dynamic resolving and signatures are resilient to game updates and obfuscation, whereas hardcoded offsets break immediately.
- **Offset Extraction & Centralization**: NEVER use `IL2CPP::Class::Utils::GetFieldOffset` in C++ for field offsets, as it breaks when games are obfuscated. Rely on `aobgenerator.py` which dynamically extracts offsets via Capstone machine-code analysis. Centralize all offsets in `Offsets.hpp`: low-level runtime structures are separated under `Offsets::Il2Cpp::...` (e.g., `Array::data`, `List::items`, `Object::cachedPtr`, `Dictionary::entryStride`), while gameplay field offsets reside under `Offsets::...` (e.g. `Offsets::alive`, `Offsets::_combatCalculator`). Never hardcode magic numbers for structs or collection layouts in hooks or helpers.
- **Signature Database**: `config.json` at the root of the project serves as the baseline signature and extraction rule database. Generic runtime collections are categorized as `Il2Cpp_<Struct>` (e.g., `Il2Cpp_Array`, `Il2Cpp_Dictionary`, `Il2Cpp_List`, `Il2Cpp_Object`). Constructor methods (`.ctor`) in `targets` are sanitized to `ctor` in C++ identifiers (e.g., `Attack_ctor`, `EntityCardDatabase_ctor`).
- **Toggles & Config**: Store feature toggles and parameters (like damage values) in `Menu::ConfigData`. Load/save these values using `config.txt` at the project binaries directory. Avoid hardcoding magic numbers for features. Always use bounded parsing (like `SafeParseInt`) to guard against unhandled exceptions on invalid config tokens.
- **Injector & Binary Scoping**: `src/Cores/injector.cpp` is strictly restricted to `EvitaniaOnline.exe` (`TARGET_PROCESS_NAME`) and `Evitania*.dll` (`PROJECT_NAME`). Foreign/global DLLs (e.g., `GameAssembly.dll`, `UnityPlayer.dll`, `dxgi.dll`) and non-target processes are rejected. Auto-discovery sorts candidates by `ftLastWriteTime` to always select the latest compiled binary. If the game process is not running, `WaitForProcess` polls every 500ms for up to 60 seconds before timing out.
- **Initialization**: Every feature file must have an `Initialize()` and `Uninitialize()` function, which is registered in `src/Modules/Hooks/Hooks.cpp`.
- **Performance**: Game loops and property getters (e.g., `get_Cost`) can be called thousands of times per second (especially during loading screens). **NEVER** use expensive operations like `std::unordered_map` lookups or heap allocations directly in the hot-path of these hooks, as it will cripple boot and framerate times. Cache or aggressively bypass these checks whenever possible.
- **Windows Header Optimization**: If including `<windows.h>`, always guard with optimization macros:
```cpp
#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#include <windows.h>
```
- **IDE Parsing (`clangd`)**: The project uses a `.clangd` file to force-include `IL2CPP_Resolver.hpp` for internal module headers. Do NOT manually add `#include "../../IL2CPP_Resolver.hpp"` to internal headers, as it causes circular dependency "incomplete type" errors in the IDE.

## Boundaries
- Do not dump game assemblies into the Git tree unless they are strictly inside `resources/dumped/`.
- Both `dump.cs` and `GameAssembly.dll` are required for maintaining updates. `build_sig_db.py` relies on `dump.cs` to generate the signature database (`config.json`), which `aobgenerator.py` then uses against `GameAssembly.dll`.
- Do not commit large compiled binaries (`.dll`, `.exe`) to version control.
- Avoid using `.hpp` to define logic; use `.hpp` for declarations and `.cpp` for implementations.
- Vendor code in `src/Modules/(ImGui|MinHook|Il2CppResolver)` must not be reformatted or modified directly, and is excluded from CodeQL and PR format checks via `.github/codeql/codeql-config.yml`.

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

### UnityEngine.Object Lifetime Validation Pattern
Unity managed objects wrap underlying C++ native engine pointers via `m_CachedPtr`. If the native object is destroyed, the managed pointer remains non-null in memory, but dereferencing native state will cause an immediate crash. Always validate object lifetime before dereferencing cached pointers:

```cpp
inline bool IsUnityObjectAlive(void* obj)
{
  if (!obj)
    return false;
  void* cachedPtr = *(void**) ((uintptr_t) obj + Offsets::Il2Cpp::Object::cachedPtr);
  return cachedPtr != nullptr;
}
```

### Gotchas / Known Quirks
- **Property Setters vs Array Removals**: When trying to hook item consumption (e.g. Infinite Items), intercepting the property setter (like `ItemDetail::set_Amount`) is often **insufficient**. The game logic often completely destroys/removes the object from the inventory array for consumables or full-stack uses by calling higher-level manager methods like `BaseStorageService::Remove` or `BaseStorageService::TryRemove`. Hook these root array managers to properly prevent removals.
- **Backing Fields in Regex**: In `build_sig_db.py` and `aobgenerator.py`, we extract field offsets via Capstone. Many unity fields are compiler-generated backing fields containing angle brackets (e.g., `<Amount>k__BackingField`). Our Python scripts have been updated to support `[a-zA-Z0-9_<>]+` in regex to correctly parse them. Do NOT regress this regex support if you edit the scripts.
- **Centralizing Offsets**: Never hardcode hex offsets in hooks (like `0x18`). ALWAYS extract them dynamically via `build_sig_db.py`'s extraction rules and place them in `Offsets.hpp`.
- **UI Visuals vs. Spending Validation (Dual-Mode Hooking & Context Scoping)**:
  - Game UI methods (e.g. `EngineerShopDescription::UpdateVisuals`, `TalentInfoPanel::SetItem`) enumerate price dictionaries (`Dictionary<ISpendable, float>`) to render currency icons. If `_count == 0`, icons vanish.
  - However, game spend resolvers (e.g. `SpendResolver`, `Trader::Spend`) enforce `if (amount <= 0) return false;`, blocking zero-cost purchases.
  - Solution: Use a `thread_local bool` flag during the purchase method (e.g. `EngineerSystemUI::TryBuyUpgrade`, `EnhancementTreeUI::Buy`) to provide an empty dictionary during the transaction (bypassing spend checks), while providing zeroed float values (`Utils::Il2Cpp::ZeroDictionaryFloatValues`) during UI rendering so icons remain visible with `0` cost.
  - **Context Isolation on Shared Pricing Methods**: When multiple game systems share a common pricing method (e.g., `MarketLotScriptableObject::CurrentPrice` shared by `EnhancementTreeUI` / Bonfire Ash and `SacrificeUI` / Sacrifice), never check both feature toggles blindly in the price getter. Track the active caller using a `thread_local` context enum set in `Select` and `Buy` hooks so each toggle only affects its own prices and spend resolvers.
- **Feature Isolation**: Never reuse general store toggles (`bFreeStore`) or currency toggles (`bInfiniteCurrency`) across subsystem progression. `InfiniteCurrency` strictly prevents subtraction of currencies (`CurrencyService::Subtract`, `HourglassService::AddResource` with `delta < 0`), while subsystem upgrade freeing (Engineer, Hunter, Bonfire, Hourglass of Eternity) must reside in its own dedicated feature module under the respective ImGui tab with its own distinct toggle.
- **GC Managed Object Reuse**: Never cache raw managed instances (e.g. `OfflineActivitySnapshot`) across calls. Unity's garbage collector frees or moves them, causing second-use memory corruption or access violation crashes. Always allow the original method to allocate a fresh, GC-tracked instance, then patch the required fields on that instance.
- **Timeskip Mob Kill Progression**: Offline timeskip rewards grant monster kills via `StatisticsService::AddKillWithPortalProgress`, but stage lock gates require specific difficulty and enemy ID matches via `MobKillTrigger::Apply` and `StatisticsService::AddPortalKill`. If triggers are inactive (e.g. player in town), queue pending kills to apply upon entering a portal stage.
- **Multiple Build Versions in Output Folder**: Never assume `ls | head -n 1` or `FindFirstFile` returns the newest build if multiple versioned DLLs exist in the build directory (e.g. `Evitania v0.320.1.dll` vs `Evitania v0.320.2.dll`). Always sort by modification timestamp (`ls -t`, `dir /o:-d`, or `CompareFileTime`) to guarantee the newest binary is loaded.
- **CodeQL Path Filtering on Manual C++ Builds**: In CodeQL, `paths-ignore` in `.github/codeql/codeql-config.yml` natively applies to interpreted languages, but manual C++ compiler tracing captures all compiled vendor files regardless. To keep CI reports clean, post-process the generated SARIF files in workflow steps to strip vendor URIs before publishing step summaries or uploading artifacts.

## Game Acknowledge
- **Trade & Economy Architecture**:
  - `ISpendable` / `IAcquirable`: Base interfaces implemented by `TradableBase` (`Currency`, `Item`, `EngineerItem`, etc.).
  - `TradeUtils.Trader`: Central router for transactions (`Trader.CanSpend`, `Trader.Spend`, `Trader.Add`).
  - Stockpile Spending: `EngineerItem` delegates spending to `EngineerService.SpendResolver`.
- **IL2CPP Memory Layouts**:
  - **Dictionary**: `_entries` at `Offsets::entries` (`0x18`), `_count` at `Offsets::count` (`0x20`). Elements start at `+0x20` inside `_entries`. Each entry is 24 bytes (`0x18`): `hashCode` (int32, +0), `next` (int32, +4), `key` (pointer, +8), `value` (float, +16).
  - **List**: `_items` array at `Offsets::Il2Cpp::List::items` (`0x10`), `_size` at `Offsets::Il2Cpp::List::size` (`0x18`). Elements in `_items` start at `Offsets::Il2Cpp::Array::data` (`+0x20`).
  - **Array**: `_length` at `Offsets::Il2Cpp::Array::length` (`0x18`), data at `Offsets::Il2Cpp::Array::data` (`0x20`).
  - **UnityEngine.Object**: `m_CachedPtr` at `Offsets::Il2Cpp::Object::cachedPtr` (`0x10`).
- **Offline & Timeskip Architecture**:
  - `OfflineProgressService`: `_combatCalculator` at `Offsets::_combatCalculator` (`0x48`), `_cardDatabase` at `Offsets::_cardDatabase` (`0x60`).
  - `CombatOfflineRewardsCalculator`: `_currentSpawner` at `Offsets::_currentSpawner` (`0x38`).
  - `EnemySpawnerService`: `_enemyCards` at `Offsets::_enemyCards` (`0x30`).
  - `EntityCardDatabase`: `enemies` list at `Offsets::enemies` (`0x18`).
  - `OfflineActivitySnapshot`: `EntityGuid` (`0x10`), `Skill` (`0x20`), `XpPerHour` (`0x24`), `KillsPerHour` (`0x28`), `GoldPerHour` (`0x2C`).
- **Stage & Map Identifiers**:
  - In `StageHopper`, Act 1 stages use numeric formatting (e.g. `1.6`), whereas boss stages and higher acts format stage identifiers as strings (e.g. `1.boss-2`). Always handle both numeric and string representations when resolving player location.
- **Boot Flow & Hot-Path Hooks**:
  - Game attaches and initializes services sequentially (`AntiCheatService`, `PositionerService`, `MapUi`, `EngineerService`).
  - Hooks running during boot or in getters called by game loops must avoid expensive string operations, unbounded pattern searches, or dynamic memory allocations to prevent boot/login screen freezes.
