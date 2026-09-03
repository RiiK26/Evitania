# Evitania Online (PC Build)
![Version](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2FItsMe-RiiK%2FEvitaniaOnline-Cheeto%2Fmain%2Fversion.json&query=%24.version&label=Version&color=green)

Evitania is an online Idle RPG with an open world, inspired by classic MMORPGs but without the endless grind. Your character continues to fight, progress, and gather resources even while you're offline, allowing you to focus on what truly matters — rare loot, meaningful upgrades, and challenging boss battles.

## Features Implemented

The project architecture is strictly modularized into feature categories within the `src/Features/` directory:

### Combat
* **God Mode**: Provides infinite health and a customizable damage multiplier to eliminate targets instantly (`AttackReceiver`).
* **Aura Kill**: Automatically and instantaneously eliminates all hostile entities loaded within the surrounding vicinity (`EnemyNpcController`).

### Economy
* **Infinite Items**: Intercepts inventory consumption logic to prevent items from being depleted upon use (`BaseStorageService`).
* **Enhance Item 100%**: Intercepts item enhancement logic to always return a 100% success rate (`EnhanceStationService`).

## Build and Run

The project has been configured to build via CMake and MinGW-w64 for Windows (compatible with Proton/Wine on Linux).

To build the project:
```bash
./scripts/build.sh
```

To run the game and inject the DLL (on Linux/Proton):
```bash
./scripts/run.sh
```

## License
This Project under [MIT License](license)
