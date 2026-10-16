## Description
<!-- Provide a brief summary of the changes introduced in this pull request and the rationale behind them. -->

## Type of Change
- [ ] 🚀 New Feature
- [ ] 🐛 Bug Fix
- [ ] 🔄 Game Update / Signature Maintenance (Steam Build ID: `________`)
- [ ] ⚡ Performance Optimization
- [ ] 🧹 Refactoring & Code Cleanup
- [ ] 📝 Documentation

## Architectural Checklist
- [ ] **Modular Architecture**: 1 feature = 1 `.hpp`/`.cpp` pair in `src/Features/<Domain>/` mapping 1:1 to the ImGui tab.
- [ ] **Lifecycle**: Feature implements `Initialize()` and `Uninitialize()`, properly hooked in `src/Modules/Hooks/Hooks.cpp`.
- [ ] **No Magic Offsets**: All struct & field offsets are centralized in `src/Modules/Hooks/Offsets.hpp` (under `Offsets::Il2Cpp::` for runtime layout or `Offsets::` for game logic).
- [ ] **No Reflection in Hot Paths**: No runtime `GetFieldOffset` calls; Capstone semantic patterns used in `config.json`.
- [ ] **Performance Conscious**: No dynamic heap allocations (`std::vector`, `std::string`, `new`) in hot paths (`get_Cost`, `Time_set_timeScale`, frame updates).
- [ ] **Object Lifetime**: Managed Unity objects dereference `m_CachedPtr` only after `IsUnityObjectAlive` validation.

## Formatting & Build Verification
- [ ] **C/C++ Formatting**: Verified against `.clang-format` (`git ls-files '*.cpp' '*.hpp' '*.h' | grep -vE 'src/Modules/(ImGui|MinHook|Il2CppResolver)/' | xargs clang-format --dry-run --Werror`).
- [ ] **Python Formatting**: Verified with Ruff / Black (`ruff format --check resources/Tools/*.py`).
- [ ] **Syntax & Schema**: `config.json` schema validated, and `python3 -m py_compile resources/Tools/*.py` passes.
- [ ] **Build Check**: `./scripts/build release` compiles cleanly with 0 errors.
- [ ] **Update Check**: `./scripts/update --check` succeeds.

## In-Game Testing
<!-- Describe how this change was tested in the running game, including any edge cases verified (e.g. stage transition, inventory sync, store pricing). -->
- Tested on Steam Build: 
- Verification Steps:
  1. 
  2. 
