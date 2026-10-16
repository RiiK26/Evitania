## Description

<!-- Describe the changes made in this PR. Why is this change necessary? What problem does it solve? -->

## Type of Change

<!-- Please check the options that apply. -->

- [ ] **Bug Fix**: Non-breaking change that fixes an existing issue.
- [ ] **New Feature**: Non-breaking addition of a new hack or menu capability.
- [ ] **Game Update**: Signature / semantic offset update for a new game build (`config.json`, `Signatures.hpp`, `Offsets.hpp`).
- [ ] **Performance**: Optimization for hot-path hooks or memory allocation.
- [ ] **Security / Hardening**: Memory safety, input boundary checks, or exploit mitigation.
- [ ] **Refactor / Code Quality**: Architectural cleanup without changing feature behavior.
- [ ] **Documentation**: Changes to docs, workflows, or templates.

## 🔗 Related Issues

<!-- Link any related issues or discussions (e.g., Fixes #123, Closes #456) -->

- Fixes #

## Architectural Checklist

<!-- Ensure your changes adhere to Evitania's architectural rules defined in CONTRIBUTING.md. -->

- [ ] **Modular Architecture**: 1 feature = 1 `.hpp`/`.cpp` pair in `src/Features/<Domain>/` (no god files).
- [ ] **UI Sync**: Directory structure maps 1:1 with the ImGui Tab names in `src/Modules/Menu/UI.cpp`.
- [ ] **Centralized Offsets**: No hardcoded hex offsets in hooks. All offsets reside in `src/Modules/Hooks/Offsets.hpp`.
- [ ] **Hooking Standards**: Used `HOOK_SIGNATURE` or `HOOK_METHOD`. Avoided fragile static memory offsets.
- [ ] **Hot-Path Performance**: Avoided heap allocations (`std::string`, `new`) and hash map lookups in high-frequency hooks/getters.
- [ ] **Unity Object Lifetime**: Validated `IsUnityObjectAlive()` before dereferencing cached native pointers.
- [ ] **Lifecycle Registered**: Feature implements `Initialize()` and `Uninitialize()` and is registered in `src/Modules/Hooks/Hooks.cpp`.


## Testing & Verification

<!-- How was this change tested? -->

- [ ] **Release Build**: Compiled cleanly locally via `./scripts/build release` (or `scripts\build release`).
- [ ] **C/C++ Code Formatting**: Checked via `.clang-format` without violations:
  ```bash
  git ls-files '*.cpp' '*.hpp' '*.h' | grep -vE 'src/Modules/(ImGui|MinHook|Il2CppResolver)/' | xargs clang-format --dry-run --Werror --Wno-error=unknown
  ```
- [ ] **Python Formatting**: Verified with `ruff format --check resources/Tools/*.py` (if python scripts were modified).
- [ ] **Signature Verification**: Validated via `./scripts/update --check` (if signatures or targets were modified).
- [ ] **In-Game Verification**: Tested in-game functionality and menu hotkey (`INSERT`) on Windows / Linux (Proton).

## Screenshots / Recordings (Optional)

<!-- If this PR introduces UI changes or in-game feature updates, add screenshots or videos here. -->
