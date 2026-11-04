/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
HOOK_METHOD:    using IL2CPPResolver @src/Modules/Il2CppResolver Module
HOOK_SIGNATURE: using Signature scanner from @src/Modules/Hooks/Signatures.hpp
HOOK_OFFSET:    using Offset from @src/Modules/Offsets.hpp
*/

#pragma once
#include "../Il2CppResolver/IL2CPP_Resolver.hpp"  // IWYU pragma: keep
#include "MinHook.h"                              // IWYU pragma: keep

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
        Hooks::bHooksFailed = true; \
      } \
    } \
    else { \
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
        Hooks::bHooksFailed = true; \
      } \
    } \
    else { \
      Hooks::bHooksFailed = true; \
    } \
  } while (0)

#define HOOK_OFFSET(OffsetName, RVA, HookFunc, OrigFuncPtr) \
  do { \
    void* target = (void*) ((uintptr_t) IL2CPP::Globals.m_GameAssembly + RVA); \
    if (target) { \
      int createStatus = MH_CreateHook(target, (LPVOID) HookFunc, (LPVOID*) &OrigFuncPtr); \
      if (createStatus != MH_OK) { \
        Hooks::bHooksFailed = true; \
      } \
    } \
    else { \
      Hooks::bHooksFailed = true; \
    } \
  } while (0)

namespace Hooks
{
  void Initialize();
  void Uninitialize();
}  // namespace Hooks
