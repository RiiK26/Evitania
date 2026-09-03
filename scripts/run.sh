#!/bin/bash
# scripts/run.sh
# Run this script to inject the built Evitania.dll into the running Evitania Online game.

set -e

# Change to project root directory
cd "$(dirname "$0")/.."

DLL_PATH="$(pwd)/build/release/Evitania.dll"
INJECTOR_EXE="$(pwd)/build/release/injector.exe"

if [ ! -f "$DLL_PATH" ] || [ ! -f "$INJECTOR_EXE" ]; then
    echo "Error: Project not built. Please run scripts/build.sh first."
    exit 1
fi

echo "Looking for EvitaniaOnline.exe..."
PID=$(pgrep -f "EvitaniaOnline.exe" | head -n 1)

if [ -z "$PID" ]; then
    echo "Error: Game is not running."
    exit 1
fi

echo "Game found at PID $PID."

# Extract STEAM_COMPAT_DATA_PATH from the process environment
COMPAT_DATA_PATH=$(strings /proc/$PID/environ | grep "^STEAM_COMPAT_DATA_PATH=" | cut -d= -f2-)
if [ -z "$COMPAT_DATA_PATH" ]; then
    echo "Error: Could not find STEAM_COMPAT_DATA_PATH for the game."
    exit 1
fi

# Extract STEAM_COMPAT_TOOL_PATHS
TOOL_PATHS=$(strings /proc/$PID/environ | grep "^STEAM_COMPAT_TOOL_PATHS=" | cut -d= -f2-)
PROTON_PATH=$(echo "$TOOL_PATHS" | cut -d: -f1)

if [ -z "$PROTON_PATH" ]; then
    echo "Error: Could not find Proton path for the game."
    exit 1
fi

PROTON_EXEC="$PROTON_PATH/proton"

if [ ! -f "$PROTON_EXEC" ]; then
    echo "Error: Proton executable not found at $PROTON_EXEC."
    exit 1
fi

# Convert Linux path to Wine "Z:" path
WINE_DLL_PATH="Z:${DLL_PATH}"
WINE_DLL_PATH="${WINE_DLL_PATH//\//\\}"

echo "STEAM_COMPAT_DATA_PATH: $COMPAT_DATA_PATH"
echo "Proton Path: $PROTON_EXEC"
echo "Injecting DLL: $WINE_DLL_PATH"

export STEAM_COMPAT_DATA_PATH="$COMPAT_DATA_PATH"

# Run the injector inside the game's prefix
"$PROTON_EXEC" runinprefix "$INJECTOR_EXE" "EvitaniaOnline.exe" "$WINE_DLL_PATH"
