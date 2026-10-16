#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>

namespace Scanner
{
  // Scans a module in memory for a byte pattern.
  // The pattern should be formatted like "48 8B C4 ? ? 58" or "48 8b c4 ?? ?? 58".
  // Returns the absolute memory address of the match, or 0 if not found.
  uintptr_t FindPattern(HMODULE hModule, const char* pattern);

  // Resolves a relative 32-bit displacement or immediate directly from machine code at runtime.
  // offset: byte offset from funcAddr to the displacement field
  // dispSize: size in bytes (1 for 8-bit, 2 for 16-bit, 4 for 32-bit displacement)
  // defaultValue: returned if funcAddr is null or memory is unreadable
  int32_t ExtractDisplacement(uintptr_t funcAddr, size_t offset, size_t dispSize = 4, int32_t defaultValue = 0);

  // Resolves RIP-relative target addresses (e.g. for jmp/call or mov rax, [rip+0x...]).
  uintptr_t ResolveRelativeAddress(uintptr_t instructionAddr, size_t instructionSize, size_t ripOffset);
}  // namespace Scanner
