#!/bin/bash
# scripts/run.sh
# Run this script to inject the built Evitania.dll into the running Evitania Online game.

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# For end user that just download from Release
if ls "$SCRIPT_DIR/"*.dll 1> /dev/null 2>&1 && [ -f "$SCRIPT_DIR/injector.exe" ]; then
    DLL_PATH=$(ls "$SCRIPT_DIR/"*.dll | head -n 1)
    INJECTOR_EXE="$SCRIPT_DIR/injector.exe"
# For user who build from source code (Release)
elif ls "$SCRIPT_DIR/../build/release/"*.dll 1> /dev/null 2>&1 && [ -f "$SCRIPT_DIR/../build/release/injector.exe" ]; then
    DLL_PATH=$(ls "$SCRIPT_DIR/../build/release/"*.dll | head -n 1)
    INJECTOR_EXE="$SCRIPT_DIR/../build/release/injector.exe"
# For user who build from source code (RelWithDebInfo / Clean)
elif ls "$SCRIPT_DIR/../build/relwithdebinfo/"*.dll 1> /dev/null 2>&1 && [ -f "$SCRIPT_DIR/../build/relwithdebinfo/injector.exe" ]; then
    DLL_PATH=$(ls "$SCRIPT_DIR/../build/relwithdebinfo/"*.dll | head -n 1)
    INJECTOR_EXE="$SCRIPT_DIR/../build/relwithdebinfo/injector.exe"
# For user who build from source code (Debug)
elif ls "$SCRIPT_DIR/../build/debug/"*.dll 1> /dev/null 2>&1 && [ -f "$SCRIPT_DIR/../build/debug/injector.exe" ]; then
    DLL_PATH=$(ls "$SCRIPT_DIR/../build/debug/"*.dll | head -n 1)
    INJECTOR_EXE="$SCRIPT_DIR/../build/debug/injector.exe"
else
    echo "Error: Could not find any .dll and injector.exe. Please build the project or extract the release archive."
    exit 1
fi

GAME_PID=""
if [ $# -gt 0 ]; then
    echo "Starting game via Steam launch option wrapper..."
    "$@" &
    GAME_PID=$!
fi

echo "Looking for EvitaniaOnline.exe..."
MAX_WAIT=60
WAITED=0
PID=""
while [ -z "$PID" ] && [ $WAITED -lt $MAX_WAIT ]; do
    PID=$(pgrep -f "EvitaniaOnline.exe" | head -n 1 || true)
    if [ -n "$PID" ]; then
        break
    fi
    sleep 0.5
    WAITED=$((WAITED + 1))
done

if [ -z "$PID" ]; then
    echo "Error: Game is not running."
    exit 1
fi

echo "Game found at PID $PID."

# Extract STEAM_COMPAT_DATA_PATH and STEAM_COMPAT_TOOL_PATHS from process environment
ENV_WAIT=0
COMPAT_DATA_PATH=""
TOOL_PATHS=""
while [ $ENV_WAIT -lt 30 ]; do
    COMPAT_DATA_PATH=$(strings /proc/$PID/environ 2>/dev/null | grep "^STEAM_COMPAT_DATA_PATH=" | cut -d= -f2- || true)
    TOOL_PATHS=$(strings /proc/$PID/environ 2>/dev/null | grep "^STEAM_COMPAT_TOOL_PATHS=" | cut -d= -f2- || true)
    if [ -n "$COMPAT_DATA_PATH" ] && [ -n "$TOOL_PATHS" ]; then
        break
    fi
    sleep 0.5
    ENV_WAIT=$((ENV_WAIT + 1))
done

if [ -z "$COMPAT_DATA_PATH" ]; then
    echo "Error: Could not find STEAM_COMPAT_DATA_PATH for the game."
    exit 1
fi

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

# If the game was just started (< 2s), give Wine prefix a brief moment to map process
PROCESS_UPTIME=$(ps -p "$PID" -o etimes= 2>/dev/null | tr -d ' ' || echo "999")
if [ -n "$PROCESS_UPTIME" ] && [ "$PROCESS_UPTIME" -lt 2 ]; then
    echo "Game just spawned, waiting 1s for Wine prefix..."
    sleep 1
fi

# Run the injector inside the game's prefix
"$PROTON_EXEC" runinprefix "$INJECTOR_EXE" "EvitaniaOnline.exe" "$WINE_DLL_PATH"

# If launched as Steam wrapper, keep running until game exits so Steam tracks the game
if [ -n "$GAME_PID" ]; then
    wait "$GAME_PID" 2>/dev/null || true
fi
