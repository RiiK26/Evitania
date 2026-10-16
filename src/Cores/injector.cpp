#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#include <windows.h>
#include <shellapi.h>
#include <iostream>
#include <string.h>
#include <tlhelp32.h>

#ifndef PROJECT_NAME
  #define PROJECT_NAME "Evitania"
#endif

#ifndef TARGET_PROCESS_NAME
  #define TARGET_PROCESS_NAME "EvitaniaOnline.exe"
#endif

static bool g_HasConsole       = false;
static bool g_IsInteractiveGui = false;

static void InitConsole(int argc)
{
  if (AttachConsole(ATTACH_PARENT_PROCESS)) {
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);
    g_HasConsole = true;
  }
  else if (argc <= 1) {
    g_IsInteractiveGui = true;
  }
}

static void LogInfo(const char* msg) { std::cout << "[+] " << msg << std::endl; }

static void LogError(const char* msg)
{
  std::cerr << "[!] " << msg << std::endl;
  if (g_IsInteractiveGui) {
    MessageBoxA(NULL, msg, "Evitania Injector", MB_OK | MB_ICONERROR);
  }
}

static DWORD GetProcessIdByName(const char* procName)
{
  HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hSnap == INVALID_HANDLE_VALUE)
    return 0;

  PROCESSENTRY32 pe;
  pe.dwSize = sizeof(pe);
  DWORD pid = 0;

  if (Process32First(hSnap, &pe)) {
    do {
      if (_stricmp(pe.szExeFile, procName) == 0) {
        pid = pe.th32ProcessID;
        break;
      }
    } while (Process32Next(hSnap, &pe));
  }

  CloseHandle(hSnap);
  return pid;
}

static DWORD WaitForProcess(const char* procName, DWORD timeoutSeconds = 60)
{
  DWORD pid = GetProcessIdByName(procName);
  if (pid != 0)
    return pid;

  char waitMsg[MAX_PATH + 64] = {};
  snprintf(waitMsg, sizeof(waitMsg), "Waiting for %s to start (timeout in %lu seconds)...", procName, timeoutSeconds);
  LogInfo(waitMsg);

  DWORD waitedMs = 0;
  while (waitedMs < timeoutSeconds * 1000) {
    Sleep(500);
    waitedMs += 500;

    pid = GetProcessIdByName(procName);
    if (pid != 0) {
      // Brief pause to allow the game to initialize its main modules
      Sleep(1500);
      return pid;
    }
  }

  return 0;
}

static const char* GetFileName(const char* path)
{
  const char* p1   = strrchr(path, '\\');
  const char* p2   = strrchr(path, '/');
  const char* last = p1 > p2 ? p1 : p2;
  return last ? (last + 1) : path;
}

static bool IsProjectDll(const char* path)
{
  if (!path || *path == '\0')
    return false;

  const char* fileName = GetFileName(path);
  size_t      projLen  = strlen(PROJECT_NAME);
  size_t      fileLen  = strlen(fileName);

  if (fileLen < projLen + 4)
    return false;

  // Must begin with PROJECT_NAME (case-insensitive)
  if (_strnicmp(fileName, PROJECT_NAME, projLen) != 0)
    return false;

  // Must end with ".dll" (case-insensitive)
  if (_stricmp(fileName + fileLen - 4, ".dll") != 0)
    return false;

  return true;
}

static bool FindProjectDllInDir(const char* dir, char* outPath, size_t outSize)
{
  if (!dir || outSize == 0)
    return false;

  char searchPattern[MAX_PATH] = {};
  if (strlen(dir) > 0) {
    snprintf(searchPattern, sizeof(searchPattern), "%s\\%s*.dll", dir, PROJECT_NAME);
  }
  else {
    snprintf(searchPattern, sizeof(searchPattern), "%s*.dll", PROJECT_NAME);
  }

  WIN32_FIND_DATAA findData;
  HANDLE           hFind = FindFirstFileA(searchPattern, &findData);
  if (hFind == INVALID_HANDLE_VALUE)
    return false;

  bool     found         = false;
  FILETIME bestWriteTime = {};

  do {
    if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (IsProjectDll(findData.cFileName)) {
        // Compare modification time so we always pick the newest build if multiple versions exist
        if (!found || CompareFileTime(&findData.ftLastWriteTime, &bestWriteTime) > 0) {
          bestWriteTime = findData.ftLastWriteTime;
          if (strlen(dir) > 0) {
            snprintf(outPath, outSize, "%s\\%s", dir, findData.cFileName);
          }
          else {
            snprintf(outPath, outSize, "%s", findData.cFileName);
          }
          found = true;
        }
      }
    }
  } while (FindNextFileA(hFind, &findData));

  FindClose(hFind);
  return found;
}

static bool ResolveProjectDll(const char* explicitPath, char* resolvedPath, size_t maxLen)
{
  if (explicitPath && explicitPath[0] != '\0') {
    if (!IsProjectDll(explicitPath))
      return false;
    snprintf(resolvedPath, maxLen, "%s", explicitPath);
    return true;
  }

  char exeDir[MAX_PATH] = {};
  GetModuleFileNameA(NULL, exeDir, MAX_PATH);
  char* p1        = strrchr(exeDir, '\\');
  char* p2        = strrchr(exeDir, '/');
  char* lastSlash = p1 > p2 ? p1 : p2;
  if (lastSlash) {
    *lastSlash = '\0';
  }
  else {
    exeDir[0] = '\0';
  }

  // 1. Search in directory of injector.exe
  if (FindProjectDllInDir(exeDir, resolvedPath, maxLen))
    return true;

  // 2. Search in current working directory
  if (FindProjectDllInDir(".", resolvedPath, maxLen))
    return true;

  // 3. Search in standard build output directories relative to injector
  char candidateDir[MAX_PATH] = {};
  snprintf(candidateDir, sizeof(candidateDir), "%s\\build\\release", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  snprintf(candidateDir, sizeof(candidateDir), "%s\\..\\build\\release", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  snprintf(candidateDir, sizeof(candidateDir), "%s\\build\\relwithdebinfo", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  snprintf(candidateDir, sizeof(candidateDir), "%s\\..\\build\\relwithdebinfo", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  snprintf(candidateDir, sizeof(candidateDir), "%s\\build\\debug", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  snprintf(candidateDir, sizeof(candidateDir), "%s\\..\\build\\debug", exeDir);
  if (FindProjectDllInDir(candidateDir, resolvedPath, maxLen))
    return true;

  return false;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  int     argc  = 0;
  LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
  if (!argvW)
    return 1;

  InitConsole(argc);

  char procName[MAX_PATH]    = TARGET_PROCESS_NAME;
  char explicitDll[MAX_PATH] = {};

  if (argc >= 2) {
    char arg1[MAX_PATH] = {};
    WideCharToMultiByte(CP_UTF8, 0, argvW[1], -1, arg1, MAX_PATH - 1, NULL, NULL);

    if (IsProjectDll(arg1)) {
      // First argument is the DLL path
      snprintf(explicitDll, sizeof(explicitDll), "%s", arg1);
    }
    else if (_stricmp(arg1, TARGET_PROCESS_NAME) == 0) {
      // First argument is the target process name
      snprintf(procName, sizeof(procName), "%s", arg1);
      if (argc >= 3) {
        char arg2[MAX_PATH] = {};
        WideCharToMultiByte(CP_UTF8, 0, argvW[2], -1, arg2, MAX_PATH - 1, NULL, NULL);
        snprintf(explicitDll, sizeof(explicitDll), "%s", arg2);
      }
    }
    else {
      LocalFree(argvW);
      char errMsg[MAX_PATH + 128] = {};
      snprintf(
        errMsg, sizeof(errMsg), "Injector is restricted to %s.\nTarget process '%s' rejected.", TARGET_PROCESS_NAME,
        arg1
      );
      LogError(errMsg);
      return 1;
    }
  }

  LocalFree(argvW);

  // Validate and resolve target DLL (must strictly belong to this project)
  char rawDllPath[MAX_PATH] = {};
  if (!ResolveProjectDll(explicitDll[0] != '\0' ? explicitDll : nullptr, rawDllPath, sizeof(rawDllPath))) {
    char errMsg[MAX_PATH + 128] = {};
    if (explicitDll[0] != '\0') {
      snprintf(
        errMsg, sizeof(errMsg), "Injector is restricted to %s project binaries (%s*.dll).\nRejected DLL: %s",
        PROJECT_NAME, PROJECT_NAME, explicitDll
      );
    }
    else {
      snprintf(
        errMsg, sizeof(errMsg), "Could not find any %s project binary (%s*.dll) in injector directory.", PROJECT_NAME,
        PROJECT_NAME
      );
    }
    LogError(errMsg);
    return 1;
  }

  char  fullDllPath[MAX_PATH] = {};
  DWORD pathLen               = GetFullPathNameA(rawDllPath, MAX_PATH, fullDllPath, nullptr);
  if (pathLen == 0 || pathLen >= MAX_PATH) {
    LogError("Failed to resolve absolute path for DLL (or path exceeds MAX_PATH).");
    return 1;
  }

  DWORD fileAttr = GetFileAttributesA(fullDllPath);
  if (fileAttr == INVALID_FILE_ATTRIBUTES || (fileAttr & FILE_ATTRIBUTE_DIRECTORY)) {
    char errMsg[MAX_PATH + 64] = {};
    snprintf(errMsg, sizeof(errMsg), "DLL file not found or is a directory: %s", fullDllPath);
    LogError(errMsg);
    return 1;
  }

  // Find target process (or wait for it to start if not already running)
  DWORD pid = WaitForProcess(procName, 60);
  if (pid == 0) {
    char errMsg[MAX_PATH + 64] = {};
    snprintf(errMsg, sizeof(errMsg), "Game process not found: %s\nTimed out waiting for game to start.", procName);
    LogError(errMsg);
    return 1;
  }

  char logBuf[MAX_PATH + 64] = {};
  snprintf(logBuf, sizeof(logBuf), "Found target process %s with PID %lu", procName, pid);
  LogInfo(logBuf);

  HANDLE hProc = OpenProcess(
    PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
    FALSE, pid
  );
  if (!hProc) {
    char errMsg[64] = {};
    snprintf(errMsg, sizeof(errMsg), "OpenProcess failed! Error: %lu", GetLastError());
    LogError(errMsg);
    return 1;
  }

  size_t fullDllPathLen = strlen(fullDllPath) + 1;
  void*  loc            = VirtualAllocEx(hProc, 0, fullDllPathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (!loc) {
    LogError("VirtualAllocEx failed!");
    CloseHandle(hProc);
    return 1;
  }

  if (!WriteProcessMemory(hProc, loc, fullDllPath, fullDllPathLen, nullptr)) {
    char errMsg[64] = {};
    snprintf(errMsg, sizeof(errMsg), "WriteProcessMemory failed! Error: %lu", GetLastError());
    LogError(errMsg);
    VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return 1;
  }

  HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE) LoadLibraryA, loc, 0, 0);
  if (!hThread) {
    char errMsg[64] = {};
    snprintf(errMsg, sizeof(errMsg), "CreateRemoteThread failed! Error: %lu", GetLastError());
    LogError(errMsg);
    VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
    CloseHandle(hProc);
    return 1;
  }

  WaitForSingleObject(hThread, INFINITE);

  DWORD exitCode = 0;
  GetExitCodeThread(hThread, &exitCode);

  VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
  CloseHandle(hThread);
  CloseHandle(hProc);

  if (exitCode == 0) {
    LogError("LoadLibraryA failed in remote process. Injection unsuccessful.");
    return 1;
  }

  snprintf(logBuf, sizeof(logBuf), "Successfully injected %s into %s", fullDllPath, procName);
  LogInfo(logBuf);

  if (g_IsInteractiveGui) {
    char successMsg[MAX_PATH + 128] = {};
    snprintf(successMsg, sizeof(successMsg), "Successfully injected %s into %s!", GetFileName(fullDllPath), procName);
    MessageBoxA(NULL, successMsg, "Evitania Injector", MB_OK | MB_ICONINFORMATION);
  }

  return 0;
}
