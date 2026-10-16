#include "Scanner.hpp"
#include <vector>
#include <cstdint>
#include <cstring>

namespace Scanner
{
  std::vector<int> ParsePattern(const char* pattern)
  {
    std::vector<int> bytes;
    const char*      start = pattern;
    const char*      end   = pattern + strlen(pattern);

    for (const char* current = start; current < end; ++current) {
      if (*current == '?') {
        bytes.push_back(-1);
        if (current + 1 < end && *(current + 1) == '?')
          ++current;  // Skip second ? in "??"
      }
      else if (*current != ' ') {
        bytes.push_back(strtoul(current, const_cast<char**>(&current), 16));
      }
    }
    return bytes;
  }

  uintptr_t FindPattern(HMODULE hModule, const char* pattern)
  {
    if (!hModule)
      return 0;

    auto dosHeader = (PIMAGE_DOS_HEADER) hModule;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
      return 0;

    auto ntHeaders = (PIMAGE_NT_HEADERS) ((uint8_t*) hModule + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
      return 0;

    uintptr_t moduleBase          = (uintptr_t) hModule;
    size_t    moduleSize          = ntHeaders->OptionalHeader.SizeOfImage;

    std::vector<int> patternBytes = ParsePattern(pattern);
    if (patternBytes.empty())
      return 0;

    size_t patternSize = patternBytes.size();
    int*   pat         = patternBytes.data();

    if (moduleSize < patternSize)
      return 0;

    uint8_t* start = (uint8_t*) moduleBase;
    uint8_t* end   = start + moduleSize - patternSize;

    if (pat[0] != -1) {
      uint8_t firstByte = (uint8_t) pat[0];
      while (start <= end) {
        start = (uint8_t*) memchr(start, firstByte, end - start + 1);
        if (!start)
          return 0;

        bool found = true;
        for (size_t j = 1; j < patternSize; ++j) {
          if (pat[j] != -1 && pat[j] != start[j]) {
            found = false;
            break;
          }
        }
        if (found) {
          return (uintptr_t) start;
        }
        start++;
      }
    }
    else {
      for (uint8_t* curr = start; curr <= end; ++curr) {
        bool found = true;
        for (size_t j = 0; j < patternSize; ++j) {
          if (pat[j] != -1 && pat[j] != curr[j]) {
            found = false;
            break;
          }
        }
        if (found) {
          return (uintptr_t) curr;
        }
      }
    }
    return 0;
  }

  int32_t ExtractDisplacement(uintptr_t funcAddr, size_t offset, size_t dispSize, int32_t defaultValue)
  {
    if (!funcAddr)
      return defaultValue;

    uintptr_t                targetAddr = funcAddr + offset;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID) targetAddr, &mbi, sizeof(mbi)) == 0)
      return defaultValue;

    if (!(mbi.State == MEM_COMMIT
          && (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))))
      return defaultValue;

    if (dispSize == 1) {
      return *(int8_t*) targetAddr;
    }
    else if (dispSize == 2) {
      return *(int16_t*) targetAddr;
    }
    else if (dispSize == 4) {
      return *(int32_t*) targetAddr;
    }
    return defaultValue;
  }

  uintptr_t ResolveRelativeAddress(uintptr_t instructionAddr, size_t instructionSize, size_t ripOffset)
  {
    if (!instructionAddr)
      return 0;

    uintptr_t                dispAddr = instructionAddr + ripOffset;
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery((LPCVOID) dispAddr, &mbi, sizeof(mbi)) == 0)
      return 0;

    if (!(mbi.State == MEM_COMMIT
          && (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE))))
      return 0;

    int32_t relOffset = *(int32_t*) dispAddr;
    return instructionAddr + instructionSize + relOffset;
  }
}  // namespace Scanner
