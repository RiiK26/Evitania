#include <windows.h>
#include <iostream>
#include <tlhelp32.h>
#include <string.h>

int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cerr << "Usage: injector.exe <process_name> <dll_path>\n";
    return 1;
  }

  const char* procName = argv[1];
  const char* dllPath  = argv[2];

  DWORD  pid   = 0;
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnap != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);
    if (Process32First(hSnap, &pe)) {
      do {
        if (_stricmp(pe.szExeFile, procName) == 0) {
          pid = pe.th32ProcessID;
          break;
        }
      } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
  }

  if (pid == 0) {
    std::cerr << "Process not found: " << procName << "\n";
    return 1;
  }

  std::cout << "Found process " << procName << " with PID " << pid << "\n";

  HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
  if (!hProc) {
    std::cerr << "OpenProcess failed! Error: " << GetLastError() << "\n";
    return 1;
  }

  void* loc = VirtualAllocEx(hProc, 0, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!loc) {
    std::cerr << "VirtualAllocEx failed!\n";
    CloseHandle(hProc);
    return 1;
  }

  WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);

  HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, loc, 0, 0);
  if (!hThread) {
    std::cerr << "CreateRemoteThread failed! Error: " << GetLastError() << "\n";
    VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return 1;
  }

  WaitForSingleObject(hThread, INFINITE);
  VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
  CloseHandle(hThread);
  CloseHandle(hProc);

  std::cout << "Successfully injected " << dllPath << "\n";
  return 0;
}
