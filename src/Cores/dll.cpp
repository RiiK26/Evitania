#include <windows.h>
#include <thread>
#include <fstream>
#include "Modules/Il2CppResolver/IL2CPP_Resolver.hpp"
#include "Modules/Hooks/Hooks.hpp"
#include "Modules/Menu/Menu.hpp"
#include "MinHook.h"

void Log(const char* msg)
{
  std::ofstream out("C:\\users\\steamuser\\Evitania.log", std::ios::app);
  out << msg << std::endl;
}

void MainThread(HMODULE hModule)
{
  Log("MainThread started.");
  if (MH_Initialize() != MH_OK) {
    Log("MH_Initialize failed.");
    return;
  }
  Log("MH_Initialize success.");

  IL2CPP::Initialize();
  Log("IL2CPP::Initialize success.");

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
