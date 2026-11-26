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

## Related Issues

<!-- Link any related issues or discussions (e.g., Fixes #123, Closes #456). Leave as "None" if not applicable. -->

- None

## Architectural Checklist

<!-- Ensure your changes adhere to Evitania's architectural rules defined in CONTRIBUTING.md (check all that apply). -->

- [ ] **Modular Architecture**: 1 feature = 1 `.hpp`/`.cpp` pair in `src/Features/<Domain>/` (no god files).
- [ ] **UI Sync**: Directory structure maps 1:1 with the ImGui Tab names in `src/Modules/Menu/UI.cpp`.
- [ ] **Centralized Offsets**: No hardcoded hex offsets in hooks. All offsets reside in `src/Modules/Hooks/Offsets.hpp`.
- [ ] **Hooking Standards**: Used `HOOK_SIGNATURE` or `HOOK_METHOD` (no ICF-merged stubs). Avoided fragile static memory offsets.
- [ ] **Hot-Path Performance**: Avoided heap allocations (`std::string`, `new`) and hash map lookups in high-frequency hooks/getters.
- [ ] **Unity Object Lifetime**: Validated `IsUnityObjectAlive()` before dereferencing cached native pointers.
- [ ] **Lifecycle Registered**: Feature implements `Initialize()` and `Uninitialize()` and is registered in `src/Modules/Hooks/Hooks.cpp`.

## Testing & Verification

<!-- How was this change tested? -->

- [ ] **Release Build**: Compiled cleanly locally via `./scripts/build release`.
- [ ] **Pre-PR Gate**: Passed `./scripts/update --verify` (`HEALTHY` doctor diagnostics, `clang-format 23.1.5`, and `ruff format --check`).
- [ ] **In-Game Verification**: Tested in-game functionality and menu hotkey (`INSERT`) on Windows / Linux (Proton).

## Screenshots / Recordings (Optional)

<!-- If this PR introduces UI changes or in-game feature updates, add screenshots or videos here. -->
