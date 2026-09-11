#pragma once
#include "../Il2CppResolver/IL2CPP_Resolver.hpp"  // IWYU pragma: keep
#include "MinHook.h"                              // IWYU pragma: keep
#include <cstdio>                                 // IWYU pragma: keep

#define HOOK_METHOD(ClassName, MethodName, ArgsCount, HookFunc, OrigFuncPtr) \
  do { \
    void* target = IL2CPP::ResolveUnityMethod(ClassName, MethodName, ArgsCount); \
    if (target) { \
      MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
    } \
  } while (0)

#include "../../Cores/Scanner.hpp"  // IWYU pragma: keep
#define HOOK_SIGNATURE(OffsetName, Signature, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signature); \
    if (target) { \
      MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
    } \
  } while (0)

#define HOOK_OFFSET(OffsetName, RVA, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) ((uintptr_t) IL2CPP::Globals.m_GameAssembly + RVA); \
    if (target) { \
      MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
    } \
  } while (0)

namespace Hooks
{
  void Initialize();
  void Uninitialize();
}  // namespace Hooks
