#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#include <windows.h>
#include <thread>
#include "Modules/Il2CppResolver/IL2CPP_Resolver.hpp"
#include "Modules/Hooks/Hooks.hpp"
#include "Modules/Menu/Menu.hpp"
#include "MinHook.h"

void MainThread(HMODULE hModule)
{
  if (MH_Initialize() != MH_OK) {
    return;
  }

  // Wait for GameAssembly.dll to be loaded in process memory
  while (!GetModuleHandleA("GameAssembly.dll")) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  // Auto-resolve IL2CPP: wait until domain is initialized and assemblies are loaded
  for (int i = 0; i < 200; ++i) {
    if (IL2CPP::Initialize(false)) {
      size_t assemblyCount = 0;
      if (IL2CPP::Domain::GetAssemblies(&assemblyCount) && assemblyCount > 5) {
        if (IL2CPP::Class::Find("UnityEngine.GameObject") != nullptr) {
          break;
        }
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  Menu::Initialize();
  Hooks::Initialize();

  MH_EnableHook(MH_ALL_HOOKS);

  while (!GetAsyncKeyState(VK_END)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  MH_DisableHook(MH_ALL_HOOKS);

  Hooks::Uninitialize();
  Menu::Uninitialize();

  MH_Uninitialize();

  FreeLibraryAndExitThread(hModule, 0);
}

void NTAPI TlsCallback(PVOID DllHandle, DWORD Reason, PVOID Reserved)
{
  if (Reason == DLL_PROCESS_ATTACH) { }
}

#ifdef _MSC_VER
  #pragma section(".CRT$XLB", read)
__declspec(allocate(".CRT$XLB")) PIMAGE_TLS_CALLBACK pTlsCallback = TlsCallback;
#else
extern "C" {
__attribute__((section(".CRT$XLB"), used)) PIMAGE_TLS_CALLBACK pTlsCallback = TlsCallback;
}
#endif

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
  switch (ul_reason_for_call) {
  case DLL_PROCESS_ATTACH :
    DisableThreadLibraryCalls(hModule);
    std::thread(MainThread, hModule).detach();
    break;
  case DLL_THREAD_ATTACH :
  case DLL_THREAD_DETACH :
    break;
  case DLL_PROCESS_DETACH :
    if (lpReserved == nullptr) {
      Hooks::Uninitialize();
      MH_Uninitialize();
    }
    break;
  }
  return TRUE;
}
