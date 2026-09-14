# Evitania Updater Tools

This directory contains the automated offset and signature extraction engine for the Evitania project.

## Future Game Update Workflow

When the game updates, the internal offsets and pointers will change, but the core assembly instructions rarely do. The new workflow entirely bypasses the need for `dump.cs` and Il2CppDumper.

Follow these steps to update the cheat after a game patch:

1. **Get the new `GameAssembly.dll`**
   Extract the updated `GameAssembly.dll` from the game's directory.
2. **Replace the old DLL**
   Place the new `GameAssembly.dll` into the `resources/dumped/` folder, overwriting the old one.
   *(Note: You do NOT need `dump.cs` anymore!)*
3. **Run the Generator**
   Navigate to the `resources/Tools/` directory and run the extraction script:
   ```bash
   python3 aobgenerator.py
   ```
   The script will:
   - Read the baseline patterns from `config.json` at the root of the project.
   - Scan the new `GameAssembly.dll` to find the target methods.
   - Use the Capstone engine to disassemble the methods and dynamically extract the new offsets.
   - Automatically rewrite `src/Modules/Hooks/Signatures.hpp` and `src/Modules/Hooks/Offsets.hpp` with the updated values.
4. **Recompile**
   Run the build script (`scripts/build` on Linux or `scripts/build.bat` on Windows) to recompile your updated `Evitania.dll`.

## How it Works

- **`config.json`**: Contains the baseline AOB signatures and Capstone extraction rules (e.g., "Look at the 16th instruction in this method and extract the operand offset").
- **`aobgenerator.py`**: The main updater script. It scans the raw machine code of `GameAssembly.dll`.
- **`find_field_usage.py` & `build_sig_db.py`**: Developer tools used to establish the baseline `config.json`. You only need to use these if you are adding brand new features to the cheat that require new offsets.
