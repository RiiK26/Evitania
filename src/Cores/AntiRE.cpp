#include "AntiRE.hpp"
#include <winternl.h>

namespace AntiRE
{

  void ErasePEHeaders(HINSTANCE hModule)
  {
    if (!hModule)
      return;

    // The base of the module in memory points to the DOS header
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) hModule;
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
      return;

    // Find the NT headers
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS) ((DWORD_PTR) hModule + pDosHeader->e_lfanew);
    if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE)
      return;

    // Change memory protection to allow overwriting the headers
    DWORD oldProtect;
    if (VirtualProtect(pDosHeader, pNtHeaders->OptionalHeader.SizeOfHeaders, PAGE_EXECUTE_READWRITE, &oldProtect)) {
      // Zero out the headers
      ZeroMemory(pDosHeader, pNtHeaders->OptionalHeader.SizeOfHeaders);

      // Restore protection
      VirtualProtect(pDosHeader, pNtHeaders->OptionalHeader.SizeOfHeaders, oldProtect, &oldProtect);
    }
  }

  bool CheckDebugger()
  {
    // 1. Basic API check
    if (IsDebuggerPresent()) {
      return true;
    }

    // 2. Remote debugger API check
    BOOL isDebuggerPresent = FALSE;
    if (CheckRemoteDebuggerPresent(GetCurrentProcess(), &isDebuggerPresent)) {
      if (isDebuggerPresent)
        return true;
    }

    // 3. Hardware Breakpoint check (Dr0-Dr3)
    CONTEXT ctx      = {0};
    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    HANDLE hThread   = GetCurrentThread();
    if (GetThreadContext(hThread, &ctx)) {
      if (ctx.Dr0 != 0 || ctx.Dr1 != 0 || ctx.Dr2 != 0 || ctx.Dr3 != 0) {
        return true;
      }
    }

// 4. Checking PEB BeingDebugged flag manually (bypasses basic IsDebuggerPresent hooks)
#ifdef _WIN64
    PPEB pPEB = (PPEB) __readgsqword(0x60);
#else
    PPEB pPEB = (PPEB) __readfsdword(0x30);
#endif
    if (pPEB->BeingDebugged == 1) {
      return true;
    }

    return false;
  }
}  // namespace AntiRE
