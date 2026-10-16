#include "StageHopper.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <windows.h>
#include <string>
#include <mutex>
#include <cstdio>

namespace Features
{
  namespace StageHopper
  {
    static void*       g_pPositionerService = nullptr;
    static std::string g_CurrentStage       = "Unknown";
    static std::mutex  g_Mutex;

    static ULONGLONG g_LastHopTick = 0;
    static bool      g_WasLooping  = false;
    enum class LoopState
    {
      ToTarget,
      ToStart
    };
    static LoopState g_LoopState = LoopState::ToTarget;

    void (*Orig_PositionerService_TryChangeScene)(void* __this, void* sceneName, bool atSpawn, void* method_info) =
      nullptr;
    void Hook_PositionerService_TryChangeScene(void* __this, void* sceneName, bool atSpawn, void* method_info)
    {
      if (__this) {
        g_pPositionerService = __this;
      }
      if (sceneName) {
        Unity::System_String* pStr = reinterpret_cast<Unity::System_String*>(sceneName);
        std::string           s    = pStr->ToString();
        if (!s.empty()) {
          std::lock_guard<std::mutex> lock(g_Mutex);
          g_CurrentStage = s;
        }
      }
      Orig_PositionerService_TryChangeScene(__this, sceneName, atSpawn, method_info);
    }

    void (*Orig_MapUi_UpdatePlayerLocation)(void* __this, void* sceneName, void* method_info) = nullptr;
    void Hook_MapUi_UpdatePlayerLocation(void* __this, void* sceneName, void* method_info)
    {
      if (sceneName) {
        Unity::System_String* pStr = reinterpret_cast<Unity::System_String*>(sceneName);
        std::string           s    = pStr->ToString();
        if (!s.empty()) {
          std::lock_guard<std::mutex> lock(g_Mutex);
          g_CurrentStage = s;
        }
      }
      Orig_MapUi_UpdatePlayerLocation(__this, sceneName, method_info);
    }

    void* GetPositioner()
    {
      if (g_pPositionerService)
        return g_pPositionerService;

      // Fallback: Resolve via IL2CPP static field
      void* pInstance = IL2CPP::Class::Utils::GetStaticField("Services.PositionerService", "Instance");
      if (pInstance) {
        g_pPositionerService = pInstance;
        return pInstance;
      }
      return nullptr;
    }

    std::string GetCurrentStage()
    {
      std::lock_guard<std::mutex> lock(g_Mutex);
      if (g_CurrentStage.empty() || g_CurrentStage == "Unknown") {
        void* pPositioner = GetPositioner();
        if (pPositioner) {
          using PfnGetCurScene                   = Unity::System_String* (*)(void*, void*);
          static PfnGetCurScene s_pfnGetCurScene = nullptr;
          if (!s_pfnGetCurScene) {
            Unity::il2cppClass* pClass = IL2CPP::Class::Find("Services.PositionerService");
            if (pClass) {
              s_pfnGetCurScene =
                reinterpret_cast<PfnGetCurScene>(IL2CPP::Class::Utils::GetMethodPointer(pClass, "get_CurrentScene", 0));
            }
            if (!s_pfnGetCurScene) {
              uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll"));
              if (base) {
                s_pfnGetCurScene = reinterpret_cast<PfnGetCurScene>(base + 0x7C6920);
              }
            }
          }
          if (s_pfnGetCurScene) {
            IL2CPP::ScopedThreadAttachment attachment;
            Unity::System_String*          pStr = s_pfnGetCurScene(pPositioner, nullptr);
            if (pStr) {
              std::string s = pStr->ToString();
              if (!s.empty()) {
                g_CurrentStage = s;
              }
            }
          }
        }
      }
      return g_CurrentStage;
    }

    void SetCurrentStage(const std::string& stageName)
    {
      std::lock_guard<std::mutex> lock(g_Mutex);
      g_CurrentStage = stageName;
    }

    bool IsLoopActive() { return Menu::Config.bStageHopperLoop; }

    bool IsValidStage(const std::string& stageName)
    {
      if (stageName.empty())
        return false;

      static bool (*s_pfnCanStreamLevel)(void*, void*) = nullptr;
      if (!s_pfnCanStreamLevel) {
        Unity::il2cppClass* pAppClass = IL2CPP::Class::Find("UnityEngine.Application");
        if (pAppClass) {
          Unity::il2cppMethodInfo* pMethod =
            IL2CPP::Class::Utils::GetMethodInfo(pAppClass, "CanStreamedLevelBeLoaded", {"System.String"});
          if (!pMethod) {
            pMethod = IL2CPP::Class::Utils::GetMethodInfo(pAppClass, "CanStreamedLevelBeLoaded", {"String"});
          }
          if (pMethod) {
            s_pfnCanStreamLevel = reinterpret_cast<bool (*)(void*, void*)>(pMethod->m_pMethodPointer);
          }
        }
        if (!s_pfnCanStreamLevel) {
          uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA("GameAssembly.dll"));
          if (base) {
            s_pfnCanStreamLevel = reinterpret_cast<bool (*)(void*, void*)>(base + 0x3969340);
          }
        }
      }

      if (!s_pfnCanStreamLevel)
        return true;  // Fallback: allow if validator could not be located

      IL2CPP::ScopedThreadAttachment attachment;
      Unity::System_String*          pStr = IL2CPP::String::New(stageName.c_str());
      if (!pStr)
        return false;

      return s_pfnCanStreamLevel(pStr, nullptr);
    }

    void HopToStage(const std::string& stageName)
    {
      if (stageName.empty())
        return;

      if (!IsValidStage(stageName))
        return;

      void* pPositioner = GetPositioner();
      if (!pPositioner || !Orig_PositionerService_TryChangeScene)
        return;

      IL2CPP::ScopedThreadAttachment attachment;

      Unity::System_String* pSceneStr = IL2CPP::String::New(stageName.c_str());
      if (!pSceneStr)
        return;

      Orig_PositionerService_TryChangeScene(pPositioner, pSceneStr, true, nullptr);
    }

    void HopOnce() { HopToStage(Menu::Config.szStageHopperTarget); }

    void Update()
    {
      if (!Menu::Config.bStageHopperLoop) {
        if (g_WasLooping) {
          g_WasLooping = false;
        }
        return;
      }

      // Check validity of target before running loop
      if (!IsValidStage(Menu::Config.szStageHopperTarget)) {
        return;
      }

      ULONGLONG currentTick = GetTickCount64();
      float     delaySec    = Menu::Config.fStageHopperDelay;
      if (delaySec < 0.5f)
        delaySec = 0.5f;
      ULONGLONG delayMs = static_cast<ULONGLONG>(delaySec * 1000.0f);

      if (!g_WasLooping) {
        // Just enabled looping
        g_WasLooping    = true;
        g_LastHopTick   = currentTick;
        g_LoopState     = LoopState::ToTarget;

        std::string cur = GetCurrentStage();
        if (!cur.empty() && cur != "Unknown") {
          snprintf(Menu::Config.szStageHopperStart, sizeof(Menu::Config.szStageHopperStart), "%s", cur.c_str());
        }
        return;
      }

      if (currentTick - g_LastHopTick >= delayMs) {
        g_LastHopTick = currentTick;

        if (g_LoopState == LoopState::ToTarget) {
          HopToStage(Menu::Config.szStageHopperTarget);
          g_LoopState = LoopState::ToStart;
        }
        else {
          HopToStage(Menu::Config.szStageHopperStart);
          g_LoopState = LoopState::ToTarget;
        }
      }
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "PositionerService::TryChangeScene", Signatures::PositionerService_TryChangeScene,
        Hook_PositionerService_TryChangeScene, Orig_PositionerService_TryChangeScene
      );

      HOOK_SIGNATURE(
        "MapUi::UpdatePlayerLocation", Signatures::MapUi_UpdatePlayerLocation, Hook_MapUi_UpdatePlayerLocation,
        Orig_MapUi_UpdatePlayerLocation
      );
    }

    void Uninitialize() { g_pPositionerService = nullptr; }
  }  // namespace StageHopper
}  // namespace Features
