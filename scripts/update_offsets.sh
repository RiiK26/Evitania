#!/bin/bash
# Wrapper script for offsetvalidator.py
# Run this script whenever you have generated a new dump.cs from a game update.

SCRIPT_DIR=$(dirname "$0")
PYTHON_SCRIPT="$SCRIPT_DIR/../resources/Tools/offsetvalidator.py"

echo "=== Evitania Offset Updater ==="
if command -v python3 &>/dev/null; then
    python3 "$PYTHON_SCRIPT"
else
    python "$PYTHON_SCRIPT"
fi
echo "==============================="
