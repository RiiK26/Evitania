# Evitania Online

![Version](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2FRiiK26%2FEvitania%2Fmain%2Fconfig.json&query=%24.version&label=Version&color=green)
![Build Status](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fapi.github.com%2Frepos%2FRiiK26%2FEvitania%2Factions%2Fruns%3Fbranch%3Dmain%26status%3Dsuccess&query=%24.workflow_runs.0.name&label=Build&color=blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20-orange)

A high-performance, modular internal client for **Evitania Online (PC Build)**. This project is engineered for resilience against application updates, leveraging dynamic IL2CPP resolution and automated Array-of-Bytes (AOB) signature extraction via static analysis.

## Features

The architecture is strictly modularized into distinct feature categories located within the `src/Features/` directory.

### Combat

| Feature | Description |
|---|---|
| **God Mode** | Grants absolute invulnerability. Includes granular configuration for health points, custom damage multipliers, and unrestricted movement speed. |
| **Aura Kill** | Automatically neutralizes all hostile entities within the localized environment radius upon rendering. |
| **Exp Multiplier** | Modifies the experience point acquisition algorithm to exponentially accelerate leveling. |
| **Speed Hack** | Modifies the global `Time.timeScale` variable to accelerate animation execution, movement physics, and application loops. |

### Economy & Progression

| Feature | Description |
|---|---|
| **Infinite Items** | Intercepts inventory consumption subroutines to prevent asset depletion upon usage. |
| **100% Enhancement Success** | Hooks the item enhancement algorithm to enforce a guaranteed 100% success probability. |
| **Infinite Currency** | Nullifies subtraction routines for all premium and standard currencies (e.g., Diamonds, Gold, Sands). |
| **Free Store (IAP Bypass)** | Bypasses client-side in-app purchase (IAP) validation checks to acquire premium marketplace items without authorization. |
| **Hourglass Bypass** | Bypasses regional and temporal cooldown restrictions for Timeskip item utilization. |

### Security & Anti-Detection

| Feature | Description |
|---|---|
| **Dynamic Signature Scanning** | Eliminates reliance on static offsets. Utilizes the Capstone Engine and Python scripts to dynamically resolve memory addresses, ensuring compatibility across application patches. |
| **Anti-Reverse Engineering** | Implements Thread Local Storage (TLS) callbacks, PEB `NtGlobalFlag` verification, `__rdtsc` timing constraints, and hypervisor detection to mitigate debugging and dynamic analysis. |
| **Memory Hardening** | Introduces bounds checking and exception handling to protect against integer underflows and denial-of-service configuration faults. |

## Tech Stack & Architecture

- **Language**: `C++20`
- **Build System**: `CMake` utilizing the `MinGW-w64` cross-compiler
- **UI Framework**: `ImGui` rendering via DirectX 11
- **Hooking**: `MinHook` for inline function interception
- **Unity Interop**: `Il2CppResolver` for dynamic Unity class and method resolution
- **Static Analysis**: `Capstone Engine` for automated machine-code offset extraction

## Build and Execution Guidelines

### Prerequisites
The build environment requires CMake, MinGW-w64, and Python 3. Linux environments support cross-compilation to a Windows `.dll`, which can be injected into the application layer via Proton.

### 1. Compilation
Execute the provided build script. This will compile the project and deploy `Evitania.dll` to the `build/release/` output directory.
```bash
./scripts/build
```

### 2. Injection (Linux / Proton)
Initialize the application via Steam. Upon reaching the primary menu interface, execute the injection script. The script automatically isolates the target process ID (PID) and Proton path for DLL injection.
```bash
./scripts/run.sh
```
Alternatively, for native Windows environments:
```bat
scripts\run.bat
```
*Note: Press the `[INSERT]` key to toggle the graphical user interface.*

## Offset Updating Protocol

Evitania maintains strict resilience across patches by utilizing Capstone to disassemble the application's machine code, dynamically extracting requisite offsets and signatures.

Following an application update:
1. Extract the latest `GameAssembly.dll` from the updated application and place it into `resources/dumped/`.
2. Execute the signature extraction script:
   **On Linux / macOS:**
   ```bash
   ./scripts/update_offsets.sh
   ```

   **On Windows:**
   ```bat
   scripts\update_offsets.bat
   ```
3. Recompile the project to integrate the newly generated `Offsets.hpp` and `Signatures.hpp`.

## License
This project is licensed under the [MIT License](license).
