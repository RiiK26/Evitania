#pragma once
#include <windows.h>

namespace AntiRE
{
  // Erases the DOS and NT headers of the current module in memory to prevent basic dumping
  void ErasePEHeaders(HINSTANCE hModule);

  // Checks for standard debuggers and hardware breakpoints
  bool CheckDebugger();

  // Checks for Virtual Machine environment using CPUID
  bool CheckVM();

  // Checks for abnormal execution delays indicating debugging/VM overhead
  bool CheckTiming();
}  // namespace AntiRE
