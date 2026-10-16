# Contributing to Evitania

This document outlines our architectural standards, development workflow, and coding conventions to ensure high quality and update resilience across the codebase.

Please review our [Code of Conduct](CODE_OF_CONDUCT.md) before participating.

## 1. Development Environment Setup

### Prerequisites
- **C++ Compiler**: MinGW-w64 (`x86_64-w64-mingw32-g++` supporting C++20) or MSVC (VS 2019/2022/2026).
- **CMake**: Version 3.20 or newer.
- **Python 3**: For signature generation and offset extraction tools.
  ```bash
  pip install capstone
  ```

### Build Commands
- **Build Release**:
  ```bash
  ./scripts/build release    # Linux
  scripts\build release      # Windows
  ```
- **Build with Debug Symbols**:
  ```bash
  ./scripts/build debug      # Linux
  scripts\build debug        # Windows
  ```
- **Check Game Update Status**:
  ```bash
  ./scripts/update --check
  ```

## 2. Core Architectural Conventions

To maintain a clean codebase that does not break across game updates, all contributions must adhere to the following conventions:

### A. Modular Feature Architecture (No God Files)
- **1 Feature = 1 `.hpp`/`.cpp` Pair**: Each feature must reside in `src/Features/<Domain>/` (e.g. `src/Features/Combat/GodMode.cpp`).
- **UI & Feature Directory Sync**: The folder structure in `src/Features/<Domain>/` **must match 1:1 with the ImGui Tab names** in `src/Modules/Menu/UI.cpp` (e.g., `Combat`, `Player`, `Economy`, `Curio`, `Engineer`, `Hunter`, `Hourglass`, `Bonfire`).
- **Lifecycle Registration**: Every feature file must implement `void Initialize()` and `void Uninitialize()`, which are registered in `src/Modules/Hooks/Hooks.cpp`.

### B. Hooking & Offset Rules
- **Prefer Byte Signatures**: Always use `HOOK_SIGNATURE(...)` with patterns from `Signatures.hpp`. Avoid hardcoded memory addresses.
- **No `GetFieldOffset` in Hot Paths**: Do not use runtime string-based reflection like `IL2CPP::Class::Utils::GetFieldOffset` in C++, as it fails when games are obfuscated and causes significant frame drops.
- **Centralize All Offsets**: All class and struct offsets must reside in `src/Modules/Hooks/Offsets.hpp`.
  - Low-level runtime collections live under `Offsets::Il2Cpp::...` (e.g., `Array::data`, `List::items`, `Dictionary::entries`).
  - Gameplay field offsets live under `Offsets::...` (e.g., `Offsets::alive`, `Offsets::discount`).
  - Never hardcode magic hex numbers (e.g. `0x18`) in hooks or helpers.
- **Semantic Assembly Patterns**: When adding extraction targets to `config.json`, use semantic assembly templates with `?` as the displacement placeholder (e.g., `"pattern": "cmp byte ptr [rcx + ?], 0"`).

### C. UI Visuals vs. Spending Validation (Dual-Mode Hooking)
- Game UI methods iterate price dictionaries (`Dictionary<ISpendable, float>`) to render currency icons. If count is 0, icons disappear from the screen.
- However, spend resolvers enforce `if (amount <= 0) return false;`, blocking zero-cost transactions.
- **Standard Solution**: Use a `thread_local bool` flag during the purchase method to provide an empty dictionary during the transaction (bypassing spend checks), while providing zeroed float values (`Utils::Il2Cpp::ZeroDictionaryFloatValues`) during UI rendering so icons remain visible with 0 cost.

### D. Hot-Path Performance
- Methods like `Time_set_timeScale`, property getters (`get_Cost`), and update loops are called thousands of times per second.
- **Never allocate heap memory** (`std::string`, `std::vector`, `new`) or perform expensive hash table lookups inside hot-path hooks.

## 3. Workflow for Adding a New Feature

When writing a feature that requires hooking a new game method or reading a new field:

1. **Disassemble & Inspect Assembly**:
   Use `find_field_usage.py` to identify the instruction and register layout:
   ```bash
   python3 resources/Tools/find_field_usage.py <Class> <Method> <Field> --disasm
   ```

2. **Add to Baseline Database**:
   Add the target to `targets` in `resources/Tools/build_sig_db.py`, then generate the extraction rules:
   ```bash
   python3 resources/Tools/build_sig_db.py
   ```

3. **Re-generate Headers & Compile**:
   ```bash
   ./scripts/update --force
   ```

4. **Implement Feature Logic**:
   Create your feature module in `src/Features/<Domain>/YourFeature.hpp` and `.cpp`, implement `Initialize()` and `Uninitialize()`, register them in `Hooks.cpp`, and add menu controls in `UI.cpp`.

## 4. Submitting Pull Requests

1. **Keep Changes Focused**: One pull request per feature or bug fix.
2. **Verify Clean Compilation**: Ensure `./scripts/build release` compiles cleanly without warnings or errors.
3. **Validate Signatures**: Ensure `./scripts/update --check` succeeds.
4. **Follow Formatting**: Keep code clean and properly formatted before opening a PR:
   - **C/C++ Formatting** (`.clang-format`):
     ```bash
     git ls-files '*.cpp' '*.hpp' '*.h' | grep -vE 'src/Modules/(ImGui|MinHook|Il2CppResolver)/' | xargs clang-format --dry-run --Werror --Wno-error=unknown
     ```
   - **Python Formatting** (`ruff` / `black`):
     ```bash
     ruff format --check resources/Tools/*.py
     ```
5. **Clear Descriptions**: In your pull request description, fill out the pull request template, explain what the feature does, how it was tested in-game, and any new signatures/offsets introduced.
