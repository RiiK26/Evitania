#!/bin/bash

SCRIPT_DIR=$(dirname "$0")
PYTHON_SCRIPT="$SCRIPT_DIR/../resources/Tools/aobgenerator.py"

echo "=== Evitania Offset Updater ==="
if command -v python3 &>/dev/null; then
    python3 "$PYTHON_SCRIPT"
else
    python "$PYTHON_SCRIPT"
fi
echo "==============================="
