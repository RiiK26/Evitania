# Evitania Online

![Version](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2FRiiK26%2FEvitania%2Fmain%2Fconfig.json&query=%24.version&label=Version&color=green)
![Build Status](https://img.shields.io/github/actions/workflow/status/RiiK26/Evitania/release.yml?branch=main)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20-orange)

## Features

The architecture is strictly modularized into distinct feature categories located within the `src/Features/` directory.

<details>
<summary> Image Preview </summary>

![image](resources/images/Preview.png)

</details>

## Build Instructions

### Linux (Cross-Compiling for Windows)
Ensure you have `mingw-w64` and `cmake` installed on your system.
```bash
sudo apt install mingw-w64 cmake
```
Then run the included build script:
```bash
./scripts/build
```

### Windows (Native)
Ensure you have `cmake` and a compatible C++ compiler (such as MinGW-w64) installed.
```cmd
mkdir build
cd build
cmake ..
cmake --build .
```

## Disclaimer

> I am not responsible for any actions taken by users of this cheat, including but not limited to violations of game terms of service or other regulations. Users should use this cheat at their own risk and be aware of the potential consequences.

## License
This project is licensed under the [MIT License](license).
