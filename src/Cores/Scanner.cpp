#include "Scanner.hpp"
#include <cstdint>
#include <cstring>

namespace Scanner
{
  struct CodeSectionInfo
  {
    HMODULE   hModule   = nullptr;
    uintptr_t codeStart = 0;
    size_t    codeSize  = 0;
  };

  static CodeSectionInfo s_cachedCode;

  uintptr_t FindPattern(HMODULE hModule, const char* pattern)
  {
    if (!hModule || !pattern)
      return 0;

    // Cache executable code section boundaries to eliminate redundant PE traversal
    if (s_cachedCode.hModule != hModule) {
      auto dosHeader = (PIMAGE_DOS_HEADER) hModule;
      if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return 0;

      auto ntHeaders = (PIMAGE_NT_HEADERS) ((uint8_t*) hModule + dosHeader->e_lfanew);
      if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        return 0;

      uintptr_t minExec = 0;
      uintptr_t maxExec = 0;
      auto      section = IMAGE_FIRST_SECTION(ntHeaders);

      for (WORD i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i, ++section) {
        if (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) {
          uintptr_t sStart = (uintptr_t) hModule + section->VirtualAddress;
          uintptr_t sEnd   = sStart + section->Misc.VirtualSize;
          if (minExec == 0 || sStart < minExec)
            minExec = sStart;
          if (sEnd > maxExec)
            maxExec = sEnd;
        }
      }

      if (minExec != 0 && maxExec > minExec) {
        s_cachedCode.codeStart = minExec;
        s_cachedCode.codeSize  = maxExec - minExec;
      }
      else {
        s_cachedCode.codeStart = (uintptr_t) hModule;
        s_cachedCode.codeSize  = ntHeaders->OptionalHeader.SizeOfImage;
      }
      s_cachedCode.hModule = hModule;
    }

    // Stack-allocated pattern buffers (zero heap allocations)
    uint8_t patBytes[512];
    uint8_t patMask[512];
    size_t  patSize              = 0;
    size_t  firstNonWildcardPos  = SIZE_MAX;
    uint8_t firstNonWildcardByte = 0;

    const char* curr             = pattern;
    while (*curr && patSize < 512) {
      if (*curr == ' ') {
        curr++;
        continue;
      }

      if (*curr == '?') {
        patBytes[patSize] = 0;
        patMask[patSize]  = 0x00;
        patSize++;
        curr++;
        if (*curr == '?')
          curr++;
      }
      else {
        char*         endPtr = nullptr;
        unsigned long val    = strtoul(curr, &endPtr, 16);
        if (endPtr == curr)
          break;
        patBytes[patSize] = (uint8_t) val;
        patMask[patSize]  = 0xFF;
        if (firstNonWildcardPos == SIZE_MAX) {
          firstNonWildcardPos  = patSize;
          firstNonWildcardByte = (uint8_t) val;
        }
        patSize++;
        curr = endPtr;
      }
    }

    if (patSize == 0 || s_cachedCode.codeSize < patSize)
      return 0;

    uint8_t* scanStart = (uint8_t*) s_cachedCode.codeStart;
    uint8_t* scanEnd   = scanStart + s_cachedCode.codeSize - patSize;

    if (firstNonWildcardPos != SIZE_MAX) {
      uint8_t* p    = scanStart + firstNonWildcardPos;
      uint8_t* pEnd = scanEnd + firstNonWildcardPos;

      while (p <= pEnd) {
        p = (uint8_t*) memchr(p, firstNonWildcardByte, pEnd - p + 1);
        if (!p)
          return 0;

        uint8_t* candidate = p - firstNonWildcardPos;
        bool     match     = true;
        for (size_t j = 0; j < patSize; ++j) {
          if ((candidate[j] ^ patBytes[j]) & patMask[j]) {
            match = false;
            break;
          }
        }
        if (match)
          return (uintptr_t) candidate;

        p++;
      }
    }
    else {
      for (uint8_t* p = scanStart; p <= scanEnd; ++p) {
        bool match = true;
        for (size_t j = 0; j < patSize; ++j) {
          if ((p[j] ^ patBytes[j]) & patMask[j]) {
            match = false;
            break;
          }
        }
        if (match)
          return (uintptr_t) p;
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

    DWORD invalidMask  = PAGE_GUARD | PAGE_NOACCESS;
    DWORD readableMask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE
                       | PAGE_EXECUTE_WRITECOPY;

    if (mbi.State != MEM_COMMIT || (mbi.Protect & readableMask) == 0 || (mbi.Protect & invalidMask) != 0)
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

    DWORD invalidMask  = PAGE_GUARD | PAGE_NOACCESS;
    DWORD readableMask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE
                       | PAGE_EXECUTE_WRITECOPY;

    if (mbi.State != MEM_COMMIT || (mbi.Protect & readableMask) == 0 || (mbi.Protect & invalidMask) != 0)
      return 0;

    int32_t relOffset = *(int32_t*) dispAddr;
    return instructionAddr + instructionSize + relOffset;
  }
}  // namespace Scanner
