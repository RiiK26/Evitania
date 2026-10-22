# Evitania

<div align="center">

[![Version](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2FRiiK26%2FEvitania%2Fmain%2Fconfig.json&query=%24.version&label=Version&color=22c55e&style=flat-square)](config.json)
[![Latest Release](https://img.shields.io/github/v/release/RiiK26/Evitania?style=flat-square&color=3b82f6)](https://github.com/RiiK26/Evitania/releases)
[![Build Status](https://img.shields.io/github/actions/workflow/status/RiiK26/Evitania/release.yml?branch=main&label=Build&style=flat-square)](https://github.com/RiiK26/Evitania/actions/workflows/release.yml)
[![CodeQL Status](https://img.shields.io/github/actions/workflow/status/RiiK26/Evitania/codeql.yml?branch=main&label=CodeQL&style=flat-square)](https://github.com/RiiK26/Evitania/actions/workflows/codeql.yml)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square&logo=c%2B%2B)](CMakeLists.txt)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20-orange?style=flat-square)](#running--injection)
[![Security Policy](https://img.shields.io/badge/Security-Policy-brightgreen?style=flat-square&logo=github)](.github/SECURITY.md)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](license)

</div>

## Table of Contents

- [Features](#features)
- [Architecture & Tech Stack](#architecture--tech-stack)
- [Quick Start Guide](#quick-start-guide)
- [Build Instructions](#build-instructions)
  - [Prerequisites](#prerequisites)
  - [Unified Build Script](#quick-build-recommended)
  - [Manual CMake Build](#manual-cmake-build)
  - [Output Binaries](#output-binaries)
- [Running & Injection](#running--injection)
- [In-Game Controls](#in-game-controls)
- [Automated Update Workflow](#automated-update-workflow)
- [Community & Contributing](#community--contributing)
- [Security](#security)
- [Support](#support)
- [Disclaimer](#disclaimer)
- [License](#license)

## Features

Evitania is engineered with a strict modular architecture (located within `src/Features/`), categorized 1:1 with the in-game ImGui interface tabs:

<details>
<summary><b>Click to view In-Game Menu Preview</b></summary>
<br>

<div align="center">

![image](resources/images/Preview.png)

</div>

</details>

### Feature Matrix

| Category | Feature | Description |
|---|---|---|
| **Combat** | **Aura Kill** | Automatically defeats surrounding monsters within a customizable radius with configurable tick rates and mob/elite/boss target filters. |
| | **God Mode** | Complete invulnerability; cancels damage calculations and prevents player health depletion. |
| **Player** | **SpeedHack** | Real-time game engine timescale multiplier for accelerated movement and actions. |
| | **Exp Multiplier** | Custom experience gain multiplier for rapid player leveling. |
| | **Stage Hopper** | Direct stage, act, and boss warp selector bypassing progression locks. |
| | **Free Enhancement** | Equipment enhancement and upgrading without consuming enhancement stones or materials. |
| **Economy** | **Infinite Currency** | Freezes currency subtraction on transactions and allows adding gold/gems. |
| | **Infinite Items** | Prevents consumable depletion and inventory stack decrement. |
| | **Free Store** | Bypasses item purchase costs in general marketplace shops. |
| **Subsystems** | **Curio Hacks** | Free curio rolls, upgrades, and progression unlocks. |
| | **Engineer Hacks** | Zero-cost building construction, workshop expansion, and stockpile upgrades. |
| | **Hunter Hacks** | Instant hunter rank unlocking and bounty reward progression. |
| | **Hourglass of Eternity**| Offline timeskip acceleration, instant progress bypass, and free upgrades. |
| | **Bonfire Hacks** | Free ash sacrifice and enhancement tree unlocks. |
| **AntiCheat** | **Bypass AntiCheat** | Neutralizes runtime integrity scans, memory monitors, and telemetry callbacks. |

## Architecture & Tech Stack

- **Modern C++20 Core**: Clean, modular codebase organized into feature domains without monolithic god-classes.
- **Dynamic Il2Cpp Resolution**: Uses `Il2CppResolver` to resolve managed classes, fields, and methods dynamically at runtime, remaining resilient to code relocation.
- **Assembly Detouring (MinHook)**: Ultra-low-overhead function interception at the machine-code level.
- **Dual-Mode Rendering (DirectX 11 & DirectX 12)**: Native Dear ImGui overlay supporting both DX11 and DX12 backends with DPI-aware font scaling.
- **Dual-Mode Purchasing Engine**: Context-scoped spending resolution via `thread_local` state tracking, enabling zero-cost transactions while preserving UI currency icons.
- **Automated Update Engine (Capstone)**: Python-driven AOB signature scanning and machine-code semantic disassembly that automatically extracts class and field offsets across game patches.

## Quick Start Guide

For end-users who downloaded a pre-compiled release from [Releases](https://github.com/RiiK26/Evitania/releases):

### Windows
1. Extract the release `.zip` archive to a folder.
2. Launch the game: `EvitaniaOnline.exe`.
3. Run `scripts\run.bat` (or open a terminal and run `injector.exe EvitaniaOnline.exe "Evitania v<version>.dll"`).
4. In-game, press <kbd>INSERT</kbd> to toggle the cheat overlay.

### Linux (Steam Proton / Steam Deck)
1. Extract the release archive.
2. Launch the game through Steam.
3. In a terminal, run:
   ```bash
   ./scripts/run.sh
   ```
4. *Alternatively*, set Steam Launch Options:
   ```text
   /path/to/scripts/run.sh %command%
   ```
5. Press <kbd>INSERT</kbd> to toggle the menu.


## Build Instructions

### Prerequisites

| Requirement | Linux (Cross-Compile) | Windows (Native) |
| :--- | :--- | :--- |
| **CMake** | `>= 3.20` | `>= 3.20` |
| **Compiler** | MinGW-w64 (`x86_64-w64-mingw32-g++` C++20) | MSVC (VS 2019/2022/2026 C++20) or MinGW-w64 |
| **Python** *(Optional)* | Python 3 + `capstone` (for updating offsets) | Python 3 + `capstone` (for updating offsets) |

#### Installing Dependencies

- **Debian / Ubuntu**:
  ```bash
  sudo apt update && sudo apt install -y mingw-w64 cmake
  ```
- **Arch Linux**:
  ```bash
  sudo pacman -S mingw-w64-gcc cmake
  ```
- **Fedora**:
  ```bash
  sudo dnf install mingw64-gcc mingw64-gcc-c++ cmake
  ```
- **Windows**:
  - Install [CMake](https://cmake.org/download/) and ensure it is added to your system `PATH`.
  - Install **Visual Studio** (with the *Desktop development with C++* workload) or [MinGW-w64 (WinLibs / MSYS2)](https://winlibs.com/).


### Quick Build (Recommended)

Clone the repository and run the cross-platform wrapper script:

```bash
git clone https://github.com/RiiK26/Evitania.git
cd Evitania
```

The unified build script ([`scripts/build`](scripts/build)) handles compilation on both Linux and Windows:

```bash
# Linux
./scripts/build release

# Windows
scripts\build release
```

#### Build Flags

| Flag | Description |
| :--- | :--- |
| `release` | *(Default)* Builds optimized, stripped binaries into `build/release/`. |
| `clean` | Builds optimized binaries with debug symbols (`RelWithDebInfo`) into `build/relwithdebinfo/`. |
| `debug` | Builds unoptimized binaries with full debug symbols (`Debug`) into `build/debug/`. |
| `no-injector` | Skips building `injector.exe`, compiling only the mod DLL. |

**Examples:**
```bash
# Build clean binary with debug symbols (outputs to build/relwithdebinfo/)
./scripts/build clean

# Build without injector
./scripts/build release no-injector
```

### Manual CMake Build

#### Linux (Cross-Compiling for Windows via MinGW)
```bash
mkdir -p build && cd build

# Release build
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
  -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
  -DCMAKE_RC_COMPILER=x86_64-w64-mingw32-windres
cmake --build . -j$(nproc)
```

#### Windows (MSVC)
```cmd
mkdir build && cd build
cmake ..
cmake --build . --config Release -j %NUMBER_OF_PROCESSORS%
```

#### Windows (MinGW-w64)
```cmd
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . -j %NUMBER_OF_PROCESSORS%
```

### Output Binaries

All build outputs are cleanly separated by configuration:

- **Release (`build/release/`)**:
  - `Evitania v<version>.dll` — Optimized and stripped production mod library.
  - `injector.exe` — Standalone injector executable.
- **RelWithDebInfo (`build/relwithdebinfo/`)**:
  - `Evitania v<version>.dll` — Optimized mod library with PDB / debug symbols.
  - `injector.exe` — Injector with debug symbols.
- **Debug (`build/debug/`)**:
  - `Evitania v<version>.dll` — Unoptimized debug binary with complete symbols.
  - `injector.exe` — Debug injector.


## Running & Injection

### 1. Linux (Steam Proton / Steam Deck)
Start the game through Steam, then run:
```bash
./scripts/run.sh

```
*The script automatically detects the running game process and its Wine/Proton prefix.*

### 2. Windows
Run the launcher script:
```cmd
scripts\run.bat
```
*Or inject manually from command line:*
```cmd
injector.exe EvitaniaOnline.exe "build\release\Evitania v<version>.dll"
```

## In-Game Controls

| Key | Action |
|:---:|---|
| <kbd>INSERT</kbd> | Toggle the ImGui cheat menu overlay. |

> [!NOTE]
> All feature toggles, sliders, and hotkeys are automatically persisted across game sessions in `config.txt` inside the .dll directory.

## Automated Update Workflow

Evitania includes an automated update pipeline designed to survive game updates with minimal manual effort:

```
Steam Update Detected
         │
         ▼
[ scripts/update --check ]  ──> Checks Steam Build ID & assembly hash
         │
         ▼
[ scripts/update ]          ──> Validates 105+ AOB byte signatures
         │                  ──> Disassembles instructions via Capstone
         │                  ──> Re-extracts semantic field offsets
         │                  ──> Updates config.json & recompiles DLL
         ▼
Ready to Play!
```

### Update Commands

1. **Check Game Update Status**:
   ```bash
   ./scripts/update --check       # Linux
   scripts\update --check         # Windows
   ```

2. **Run End-to-End Update & Rebuild**:
   ```bash
   ./scripts/update               # Linux
   scripts\update                 # Windows
   ```

3. **Live RAM Extraction (For Packed / Encrypted Updates)**:
   If the game executable is protected or encrypted on disk, run the game and extract directly from memory:
   ```bash
   ./scripts/update --from-mem    # Linux
   scripts\update --from-mem      # Windows
   ```

*For more details on reverse engineering tools and symbol deobfuscation, see [Tools Documentation](resources/Tools/README.md).*

## Community & Contributing

Contributions are welcome! Please read our contributing guidelines before submitting code:

- **[Contributing Guide](CONTRIBUTING.md)**: Development workflow, modular architecture conventions, and offset centralization rules.
- **[Code of Conduct](CODE_OF_CONDUCT.md)**: Community standards and expectations.
- **[Pull Request Template](.github/pull_request_template.md)**: Standard template and PR verification checklist.
- **[Issue Templates](.github/ISSUE_TEMPLATE/)**: Bug reports and game update requests.

## Security

We take software security and integrity seriously. For vulnerability reporting guidelines, threat model details, and supported versions, please consult our **[Security Policy](.github/SECURITY.md)**.

Please report suspected vulnerabilities privately via **[GitHub Private Advisories](https://github.com/RiiK26/Evitania/security/advisories/new)** rather than filing public issues.

## Support

If you like this project and want to support its development, consider buying me a coffee or donating!

<details>
<summary><b>Donation Addresses</b></summary>

| Payment Method | Address |
| :--- | :--- |
| ![Bitcoin](https://img.shields.io/badge/Bitcoin-000000?style=for-the-badge&logo=bitcoin&logoColor=white) | `bc1qpp50c2wuz5n2rq9jy3fxdmte7smcwu5rnegu6q` |
| ![Ethereum](https://img.shields.io/badge/Ethereum-3C3C3D?style=for-the-badge&logo=ethereum&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Tether](https://img.shields.io/badge/Tether-14B693?style=for-the-badge&logo=tether&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![BNB](https://img.shields.io/badge/BNB-F3BA2F?style=for-the-badge&logo=binance&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![XRP](https://img.shields.io/badge/XRP-23292F?style=for-the-badge&logo=xrp&logoColor=white) | `rnDnG9QBce7sbmY86HXqBUpcCq6LN3xfXg` |
| ![USDC](https://img.shields.io/badge/USDC-2775CA?style=for-the-badge&logo=usd-coin&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Solana](https://img.shields.io/badge/Solana-000000?style=for-the-badge&logo=solana&logoColor=blue) | `FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2` |
| ![Tron](https://img.shields.io/badge/Tron-FF0013?style=for-the-badge&logo=tron&logoColor=white) | `TYNLxxQWERit64uNo8dSQX3CxdLmomtAq7` |
| ![Dogecoin](https://img.shields.io/badge/Dogecoin-C2A633?style=for-the-badge&logo=dogecoin&logoColor=white) | `DFwJXDQsqPEdpnWhXCMmBWMru9iqYqWhDn` |
| ![Cardano](https://img.shields.io/badge/Cardano-0033AD?style=for-the-badge&logo=cardano&logoColor=white) | `addr1qyhapduj2uvu8x4ct75hujtsx63uq375cxda8m25xgn7wep06zmey4cecwdtshaf0eyhqd4rcprafsvm60k4gv38uajq2z2l6a` |
| ![Litecoin](https://img.shields.io/badge/Litecoin-A6A9AA?style=for-the-badge&logo=litecoin&logoColor=black) | `LMNbzEJ3M4qtAxsYyxMBkT4rzjSBJvUc2M` |
| ![Avalanche](https://img.shields.io/badge/Avalanche-E84142?style=for-the-badge&logo=avalanche&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Polkadot](https://img.shields.io/badge/Polkadot-E6007A?style=for-the-badge&logo=polkadot&logoColor=white) | `15FtRzNuwpbNAjvMzRCegqDwN7cgw2a44ogfobdS7UmNACFr` |
| ![Polygon](https://img.shields.io/badge/Polygon-8247E5?style=for-the-badge&logo=polygon&logoColor=white) | `0xbF16e9cC4F75Dcd5c1DaD4443b9d8348eC196592` |
| ![Cosmos](https://img.shields.io/badge/Cosmos-2E3148?style=for-the-badge&logo=cosmos&logoColor=white) | `cosmos1ashcczkgj884t9gert2se4m4zw5ea7t2fa4qh9` |
| ![X0 Cash](https://img.shields.io/badge/X0_Cash-228B22?style=for-the-badge&logoColor=white) | `FLFVbCaYQWoPrm9rH1WLuoYmiVnC5CFWoCPzkNk2vzy2` |

</details>

## Disclaimer

> This project is developed strictly for educational and reverse-engineering research purposes. The developers do not promote cheating in online environments and assume no responsibility for any actions taken by users, including account suspensions or bans by game publishers. Use at your own discretion.

## License

This project is licensed under the [MIT License](license).
