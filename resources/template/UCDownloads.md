Protector wanted: VMProtect
Settings: Normal / Mutation Only (Do NOT use Ultra/Virtualization globally)

Additional Options:
- Memory Protection: No (Required for MinHook to work properly)
- Import Protection: Yes
- Resource Protection: Yes
- Anti-Debug: Yes
- Anti-Dump: Yes
- Virtualization Tools detection: No

Note to UCDownloads Staff:
Please avoid applying heavy Virtualization to the entire code section, as this DLL contains real-time game hooks (like DirectX Present and Unity Update loops) that will cause severe FPS drops/delays if virtualized. Mutation is preferred for the main logic. Memory protection must be disabled so our hooking library (MinHook) can successfully write trampoline jumps in memory without triggering memory-corruption exceptions. Thank you!
