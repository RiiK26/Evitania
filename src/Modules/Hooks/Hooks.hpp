/*
HOOK_METHOD:    using IL2CPPResolver @src/Modules/Il2CppResolver Module
HOOK_SIGNATURE: using Signature scanner from @src/Modules/Hooks/Signatures.hpp
HOOK_OFFSET:    using Offset Scanner from @src/Modules/Offsets.hpp (Not used for now)
*/

#pragma once
#include "../Il2CppResolver/IL2CPP_Resolver.hpp"  // IWYU pragma: keep
#include "MinHook.h"                              // IWYU pragma: keep
#include "../Menu/Logger.hpp"                     // IWYU pragma: keep

namespace Hooks
{
  extern bool bHooksFailed;
}

#define HOOK_METHOD(ClassName, MethodName, ArgsCount, HookFunc, OrigFuncPtr) \
  do { \
    void* target = IL2CPP::ResolveUnityMethod(ClassName, MethodName, ArgsCount); \
    if (target) { \
      int createStatus = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      if (createStatus != MH_OK) { \
        Menu::Logger::Log( \
          "[HOOK_METHOD] MH_CreateHook failed (%d) for %s::%s\n", createStatus, ClassName, MethodName \
        ); \
        Hooks::bHooksFailed = true; \
      } \
      else { \
        int enableStatus = MH_EnableHook(target); \
        if (enableStatus != MH_OK) { \
          Menu::Logger::Log( \
            "[HOOK_METHOD] MH_EnableHook failed (%d) for %s::%s\n", enableStatus, ClassName, MethodName \
          ); \
          Hooks::bHooksFailed = true; \
        } \
      } \
    } \
    else { \
      Menu::Logger::Log("[HOOK_METHOD] method not found: %s::%s\n", ClassName, MethodName); \
      Hooks::bHooksFailed = true; \
    } \
  } while (0)

#include "../../Cores/Scanner.hpp"  // IWYU pragma: keep
#define HOOK_SIGNATURE(OffsetName, Signature, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signature); \
    if (target) { \
      int createStatus = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      if (createStatus != MH_OK) { \
        Menu::Logger::Log("[HOOK_SIGNATURE] MH_CreateHook failed (%d) for pattern %s\n", createStatus, OffsetName); \
        Hooks::bHooksFailed = true; \
      } \
      else { \
        int enableStatus = MH_EnableHook(target); \
        if (enableStatus != MH_OK) { \
          Menu::Logger::Log("[HOOK_SIGNATURE] MH_EnableHook failed (%d) for pattern %s\n", enableStatus, OffsetName); \
          Hooks::bHooksFailed = true; \
        } \
      } \
    } \
    else { \
      Menu::Logger::Log("[HOOK_SIGNATURE] pattern not found: %s\n", OffsetName); \
      Hooks::bHooksFailed = true; \
    } \
  } while (0)

#define HOOK_OFFSET(OffsetName, RVA, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) ((uintptr_t) IL2CPP::Globals.m_GameAssembly + RVA); \
    if (target) { \
      int createStatus = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      if (createStatus != MH_OK) { \
        Menu::Logger::Log("[HOOK_OFFSET] MH_CreateHook failed (%d) for %s\n", createStatus, OffsetName); \
        Hooks::bHooksFailed = true; \
      } \
      else { \
        int enableStatus = MH_EnableHook(target); \
        if (enableStatus != MH_OK) { \
          Menu::Logger::Log("[HOOK_OFFSET] MH_EnableHook failed (%d) for %s\n", enableStatus, OffsetName); \
          Hooks::bHooksFailed = true; \
        } \
      } \
    } \
    else { \
      Menu::Logger::Log("[HOOK_OFFSET] target address invalid for %s\n", OffsetName); \
      Hooks::bHooksFailed = true; \
    } \
  } while (0)

namespace Hooks
{
  void Initialize();
  void Uninitialize();
}  // namespace Hooks
