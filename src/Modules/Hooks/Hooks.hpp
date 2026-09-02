#pragma once
#include "../Il2CppResolver/IL2CPP_Resolver.hpp"
#include "MinHook.h"
#include <cstdio>

extern void Log(const char* msg);

#define HOOK_METHOD(ClassName, MethodName, ArgsCount, HookFunc, OrigFuncPtr) \
  do { \
    void* target = IL2CPP::ResolveUnityMethod(ClassName, MethodName, ArgsCount); \
    if (target) { \
      MH_STATUS s = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      char      buf[128]; \
      snprintf(buf, sizeof(buf), "Hooking %s::%s -> %s", ClassName, MethodName, s == MH_OK ? "Success" : "Failed"); \
      Log(buf); \
    } \
    else { \
      char buf[128]; \
      snprintf(buf, sizeof(buf), "Failed to resolve %s::%s", ClassName, MethodName); \
      Log(buf); \
    } \
  } while (0)

#define HOOK_OFFSET(OffsetName, RVA, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) ((uintptr_t) IL2CPP::Globals.m_GameAssembly + RVA); \
    if (target) { \
      MH_STATUS s = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      char      buf[128]; \
      snprintf(buf, sizeof(buf), "Hooking Offset %s -> %s", OffsetName, s == MH_OK ? "Success" : "Failed"); \
      Log(buf); \
    } \
    else { \
      char buf[128]; \
      snprintf(buf, sizeof(buf), "Failed to resolve Offset %s", OffsetName); \
      Log(buf); \
    } \
  } while (0)

namespace Hooks
{
  void Initialize();
  void Uninitialize();
}  // namespace Hooks
