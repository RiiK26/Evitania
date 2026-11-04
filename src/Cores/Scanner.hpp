/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

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
