#include "HourglassBypass.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

#include <algorithm>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Features
{
  namespace HourglassBypass
  {
    bool (*Orig_TimeskipItem_get_CanUseImpl)(void* __this, void* method_info);
    bool Hook_TimeskipItem_get_CanUseImpl(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassBypass) {
        return true;
      }
      return Orig_TimeskipItem_get_CanUseImpl(__this, method_info);
    }

    bool (*Orig_OfflineProgressService_CanUseTimeskip)(void* __this, void* method_info);
    bool Hook_OfflineProgressService_CanUseTimeskip(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassBypass) {
        return true;
      }
      return Orig_OfflineProgressService_CanUseTimeskip(__this, method_info);
    }

    static void*              s_LastValidSnapshot = nullptr;
    static std::vector<void*> s_ActiveTriggers;
    static std::mutex         s_TriggerMutex;
    static int64_t            s_PendingTimeskipPortalKills = 0;
    static void*              s_StatisticsService          = nullptr;

    inline bool IsSnapshotValid(void* snapshot)
    {
      if (!snapshot)
        return false;
      void* entityGuid = *(void**) ((uintptr_t) snapshot + Offsets::EntityGuid);
      if (!entityGuid)
        return false;
      int len = *(int*) ((uintptr_t) entityGuid + 0x10);
      if (len <= 0)
        return false;
      float xpPerHour = *(float*) ((uintptr_t) snapshot + Offsets::XpPerHour);
      return xpPerHour > 0.0f;
    }

    void* (*Orig_OfflineProgressService_CurrentActivitySnapshot)(void* __this, void* method_info) = nullptr;
    void* Hook_OfflineProgressService_CurrentActivitySnapshot(void* __this, void* method_info)
    {
      return Orig_OfflineProgressService_CurrentActivitySnapshot(__this, method_info);
    }

    void (*Orig_OfflineProgressService_SetCurrentActivity)(void* __this, void* snapshot, void* method_info);
    void Hook_OfflineProgressService_SetCurrentActivity(void* __this, void* snapshot, void* method_info)
    {
      if (snapshot && IsSnapshotValid(snapshot)) {
        s_LastValidSnapshot = snapshot;
      }
      else if (Menu::Config.bHourglassBypass && (!snapshot || !IsSnapshotValid(snapshot))) {
        // Prevent game from clearing active activity on town/gathering exit
        return;
      }
      Orig_OfflineProgressService_SetCurrentActivity(__this, snapshot, method_info);
    }

    void (*Orig_OfflineProgressService_WriteClearedActivity)(void* __this, void* method_info);
    void Hook_OfflineProgressService_WriteClearedActivity(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassBypass) {
        // Prevent game from wiping saved activity in hero profile
        return;
      }
      Orig_OfflineProgressService_WriteClearedActivity(__this, method_info);
    }

    void* (*Orig_OfflineProgressService_CreateActivitySnapshot)(void* __this, void* method_info);
    void* Hook_OfflineProgressService_CreateActivitySnapshot(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassBypass) {
        // 1. Try real-time activity evaluation for active combat/gathering stage
        if (Orig_OfflineProgressService_CurrentActivitySnapshot && __this) {
          void* cur = Orig_OfflineProgressService_CurrentActivitySnapshot(__this, nullptr);
          if (IsSnapshotValid(cur)) {
            s_LastValidSnapshot = cur;
            return cur;
          }
        }

        // 2. Try previous valid snapshot from current game session
        if (IsSnapshotValid(s_LastValidSnapshot)) {
          return s_LastValidSnapshot;
        }
      }

      void* snapshot = Orig_OfflineProgressService_CreateActivitySnapshot(__this, method_info);
      if (Menu::Config.bHourglassBypass) {
        if (IsSnapshotValid(snapshot)) {
          s_LastValidSnapshot = snapshot;
          return snapshot;
        }

        // 3. Fallback: synthesize valid snapshot so Calculate() never returns NULL
        if (snapshot) {
          void* enemyUniqueName = nullptr;

          // Priority A: Target enemy of active stage lock portal trigger
          {
            std::lock_guard<std::mutex> lock(s_TriggerMutex);
            for (void* trigger : s_ActiveTriggers) {
              if (!trigger)
                continue;
              void* enemyCard = *(void**) ((uintptr_t) trigger + Offsets::enemy);
              if (!enemyCard)
                continue;
              void* uName = *(void**) ((uintptr_t) enemyCard + Offsets::uniqueName);
              if (uName) {
                enemyUniqueName = uName;
                break;
              }
            }
          }

          // Priority B: Target enemy from combatCalculator._currentSpawner
          if (!enemyUniqueName && __this) {
            void* combatCalc = *(void**) ((uintptr_t) __this + 0x48);
            if (combatCalc) {
              void* spawner = *(void**) ((uintptr_t) combatCalc + 0x38);
              if (spawner) {
                void* cardsList = *(void**) ((uintptr_t) spawner + 0x30);
                if (cardsList) {
                  void* items = *(void**) ((uintptr_t) cardsList + 0x10);
                  int   count = *(int*) ((uintptr_t) cardsList + Offsets::Il2Cpp::Array::length);
                  if (items && count > 0) {
                    void* card = *(void**) ((uintptr_t) items + Offsets::Il2Cpp::Array::data);
                    if (card) {
                      enemyUniqueName = *(void**) ((uintptr_t) card + Offsets::uniqueName);
                    }
                  }
                }
              }
            }
          }

          // Priority C: First registered enemy from _cardDatabase
          if (!enemyUniqueName && __this) {
            void* cardDb = *(void**) ((uintptr_t) __this + 0x60);
            if (cardDb) {
              void* enemiesList = *(void**) ((uintptr_t) cardDb + 0x18);
              if (enemiesList) {
                void* items = *(void**) ((uintptr_t) enemiesList + 0x10);
                int   count = *(int*) ((uintptr_t) enemiesList + Offsets::Il2Cpp::Array::length);
                if (items && count > 0) {
                  void* card = *(void**) ((uintptr_t) items + Offsets::Il2Cpp::Array::data);
                  if (card) {
                    enemyUniqueName = *(void**) ((uintptr_t) card + Offsets::uniqueName);
                  }
                }
              }
            }
          }

          if (enemyUniqueName) {
            *(void**) ((uintptr_t) snapshot + Offsets::EntityGuid) = enemyUniqueName;
            float xp = *(float*) ((uintptr_t) snapshot + Offsets::XpPerHour);
            if (xp <= 0.0f) {
              *(float*) ((uintptr_t) snapshot + Offsets::XpPerHour) = 50000.0f;
            }
            float kills = *(float*) ((uintptr_t) snapshot + Offsets::KillsPerHour);
            if (kills <= 0.0f) {
              *(float*) ((uintptr_t) snapshot + Offsets::KillsPerHour) = 250.0f;
            }
            float gold = *(float*) ((uintptr_t) snapshot + Offsets::GoldPerHour);
            if (gold <= 0.0f) {
              *(float*) ((uintptr_t) snapshot + Offsets::GoldPerHour) = 10000.0f;
            }
            *(int32_t*) ((uintptr_t) snapshot + 0x20) = 0;  // SkillName::Combat
            s_LastValidSnapshot                       = snapshot;
            return snapshot;
          }
        }
      }
      return snapshot;
    }

    // Helper to compare IL2CPP System_String pointers
    inline bool AreStringsEqual(void* strA, void* strB)
    {
      if (strA == strB)
        return true;
      if (!strA || !strB)
        return false;
      auto* sA = reinterpret_cast<Unity::System_String*>(strA);
      auto* sB = reinterpret_cast<Unity::System_String*>(strB);
      return sA->ToString() == sB->ToString();
    }

    // MobKillTrigger lifetime hooks to track active stage lock portals
    void (*Orig_MobKillTrigger_OnEnable)(void* __this, void* method_info) = nullptr;
    void Hook_MobKillTrigger_OnEnable(void* __this, void* method_info)
    {
      if (__this) {
        std::lock_guard<std::mutex> lock(s_TriggerMutex);
        if (std::find(s_ActiveTriggers.begin(), s_ActiveTriggers.end(), __this) == s_ActiveTriggers.end()) {
          s_ActiveTriggers.push_back(__this);
        }
      }
      Orig_MobKillTrigger_OnEnable(__this, method_info);
    }

    void (*Orig_MobKillTrigger_OnDisable)(void* __this, void* method_info) = nullptr;
    void Hook_MobKillTrigger_OnDisable(void* __this, void* method_info)
    {
      if (__this) {
        std::lock_guard<std::mutex> lock(s_TriggerMutex);
        auto                        it = std::find(s_ActiveTriggers.begin(), s_ActiveTriggers.end(), __this);
        if (it != s_ActiveTriggers.end()) {
          s_ActiveTriggers.erase(it);
        }
      }
      Orig_MobKillTrigger_OnDisable(__this, method_info);
    }

    void (*Orig_MobKillTrigger_Apply)(void* __this, int64_t mobkills, void* method_info) = nullptr;
    void Hook_MobKillTrigger_Apply(void* __this, int64_t mobkills, void* method_info)
    {
      Orig_MobKillTrigger_Apply(__this, mobkills, method_info);
    }

    void (*Orig_StatisticsService_AddPortalKill)(
      void* __this, int32_t difficulty, void* enemyId, int64_t amount, void* method_info
    ) = nullptr;
    void Hook_StatisticsService_AddPortalKill(
      void* __this, int32_t difficulty, void* enemyId, int64_t amount, void* method_info
    )
    {
      if (__this) {
        s_StatisticsService = __this;
      }
      Orig_StatisticsService_AddPortalKill(__this, difficulty, enemyId, amount, method_info);
    }

    void (*Orig_MobKillTrigger_Start)(void* __this, void* method_info) = nullptr;
    void Hook_MobKillTrigger_Start(void* __this, void* method_info)
    {
      if (__this) {
        std::lock_guard<std::mutex> lock(s_TriggerMutex);
        if (std::find(s_ActiveTriggers.begin(), s_ActiveTriggers.end(), __this) == s_ActiveTriggers.end()) {
          s_ActiveTriggers.push_back(__this);
        }
      }

      if (Menu::Config.bHourglassBypass && __this && s_PendingTimeskipPortalKills > 0) {
        void* enemyCard = *(void**) ((uintptr_t) __this + Offsets::enemy);
        if (enemyCard) {
          void*   portalEnemyId = *(void**) ((uintptr_t) enemyCard + Offsets::uniqueName);
          int32_t portalDiff    = *(int32_t*) ((uintptr_t) __this + Offsets::difficulty);
          if (portalEnemyId) {
            void* pStatService = s_StatisticsService;
            if (!pStatService) {
              pStatService = IL2CPP::Class::Utils::GetStaticField("Services.StatisticsService", "Instance");
            }
            if (pStatService && Orig_StatisticsService_AddPortalKill) {
              Orig_StatisticsService_AddPortalKill(
                pStatService, portalDiff, portalEnemyId, s_PendingTimeskipPortalKills, nullptr
              );
            }
            int32_t reqKills = *(int32_t*) ((uintptr_t) __this + Offsets::mobKillsToOpen);
            if (s_PendingTimeskipPortalKills >= reqKills && Orig_MobKillTrigger_Apply) {
              Orig_MobKillTrigger_Apply(__this, s_PendingTimeskipPortalKills, nullptr);
            }
          }
        }
        s_PendingTimeskipPortalKills = 0;
      }
      Orig_MobKillTrigger_Start(__this, method_info);
    }

    void (*Orig_StatisticsService_AddKillWithPortalProgress)(
      void* __this, int32_t difficulty, void* enemyId, int64_t amount, void* method_info
    ) = nullptr;
    void Hook_StatisticsService_AddKillWithPortalProgress(
      void* __this, int32_t difficulty, void* enemyId, int64_t amount, void* method_info
    )
    {
      if (__this) {
        s_StatisticsService = __this;
      }

      Orig_StatisticsService_AddKillWithPortalProgress(__this, difficulty, enemyId, amount, method_info);

      if (Menu::Config.bHourglassBypass && amount > 0) {
        std::lock_guard<std::mutex> lock(s_TriggerMutex);
        if (s_ActiveTriggers.empty()) {
          // If in town or a scene without triggers, store pending kills so the next stage entered gets them
          s_PendingTimeskipPortalKills += amount;
        }
        else {
          for (void* trigger : s_ActiveTriggers) {
            if (!trigger)
              continue;
            void* enemyCard = *(void**) ((uintptr_t) trigger + Offsets::enemy);
            if (!enemyCard)
              continue;
            void*   portalEnemyId = *(void**) ((uintptr_t) enemyCard + Offsets::uniqueName);
            int32_t portalDiff    = *(int32_t*) ((uintptr_t) trigger + Offsets::difficulty);
            if (portalEnemyId) {
              if (
                (!AreStringsEqual(portalEnemyId, enemyId) || portalDiff != difficulty)
                && Orig_StatisticsService_AddPortalKill
              ) {
                Orig_StatisticsService_AddPortalKill(__this, portalDiff, portalEnemyId, amount, nullptr);
              }
              int32_t reqKills = *(int32_t*) ((uintptr_t) trigger + Offsets::mobKillsToOpen);
              if (amount >= reqKills && Orig_MobKillTrigger_Apply) {
                Orig_MobKillTrigger_Apply(trigger, amount, nullptr);
              }
            }
          }
        }
      }
    }

    int32_t (*Orig_HourglassTalentConfig_get_Branch)(void* __this, void* method_info);

    std::unordered_map<void*, double> originalTalentCosts;

    double (*Orig_HourglassTalentConfig_get_Cost)(void* __this, void* method_info);
    double Hook_HourglassTalentConfig_get_Cost(void* __this, void* method_info)
    {
      if (__this) {
        double* pCost = (double*) ((uintptr_t) __this + Offsets::cost);

        if (Menu::Config.bFreeTimelineNodes || Menu::Config.bFreeRestorationNodes) {
          if (*pCost != 0.0) {
            originalTalentCosts[__this] = *pCost;
            *pCost                      = 0.0;
          }
        }
        else {
          if (*pCost == 0.0) {
            auto it = originalTalentCosts.find(__this);
            if (it != originalTalentCosts.end()) {
              *pCost = it->second;
            }
          }
        }
      }
      return Orig_HourglassTalentConfig_get_Cost(__this, method_info);
    }

    void* (*Orig_HourglassSystemUI_BuildTalentTooltipContent)(void* __this, void* talent, void* method_info);
    void* Hook_HourglassSystemUI_BuildTalentTooltipContent(void* __this, void* talent, void* method_info)
    {
      if (talent) {
        double* pCost = (double*) ((uintptr_t) talent + Offsets::cost);

        if (Menu::Config.bFreeTimelineNodes || Menu::Config.bFreeRestorationNodes) {
          if (*pCost != 0.0) {
            originalTalentCosts[talent] = *pCost;
            *pCost                      = 0.0;
          }
        }
        else {
          if (*pCost == 0.0) {
            auto it = originalTalentCosts.find(talent);
            if (it != originalTalentCosts.end()) {
              *pCost = it->second;
            }
          }
        }
      }
      return Orig_HourglassSystemUI_BuildTalentTooltipContent(__this, talent, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "TimeskipItem::get_CanUseImpl", Signatures::TimeskipItem_get_CanUseImpl, Hook_TimeskipItem_get_CanUseImpl,
        Orig_TimeskipItem_get_CanUseImpl
      );

      HOOK_SIGNATURE(
        "OfflineProgressService::CanUseTimeskip", Signatures::OfflineProgressService_CanUseTimeskip,
        Hook_OfflineProgressService_CanUseTimeskip, Orig_OfflineProgressService_CanUseTimeskip
      );

      HOOK_SIGNATURE(
        "OfflineProgressService::SetCurrentActivity", Signatures::OfflineProgressService_SetCurrentActivity,
        Hook_OfflineProgressService_SetCurrentActivity, Orig_OfflineProgressService_SetCurrentActivity
      );

      HOOK_SIGNATURE(
        "OfflineProgressService::WriteClearedActivity", Signatures::OfflineProgressService_WriteClearedActivity,
        Hook_OfflineProgressService_WriteClearedActivity, Orig_OfflineProgressService_WriteClearedActivity
      );

      HOOK_SIGNATURE(
        "OfflineProgressService::CreateActivitySnapshot", Signatures::OfflineProgressService_CreateActivitySnapshot,
        Hook_OfflineProgressService_CreateActivitySnapshot, Orig_OfflineProgressService_CreateActivitySnapshot
      );

      HOOK_SIGNATURE(
        "OfflineProgressService::CurrentActivitySnapshot", Signatures::OfflineProgressService_CurrentActivitySnapshot,
        Hook_OfflineProgressService_CurrentActivitySnapshot, Orig_OfflineProgressService_CurrentActivitySnapshot
      );

      HOOK_SIGNATURE(
        "MobKillTrigger::OnEnable", Signatures::MobKillTrigger_OnEnable, Hook_MobKillTrigger_OnEnable,
        Orig_MobKillTrigger_OnEnable
      );

      HOOK_SIGNATURE(
        "MobKillTrigger::OnDisable", Signatures::MobKillTrigger_OnDisable, Hook_MobKillTrigger_OnDisable,
        Orig_MobKillTrigger_OnDisable
      );

      HOOK_SIGNATURE(
        "MobKillTrigger::Apply", Signatures::MobKillTrigger_Apply, Hook_MobKillTrigger_Apply, Orig_MobKillTrigger_Apply
      );

      HOOK_SIGNATURE(
        "MobKillTrigger::Start", Signatures::MobKillTrigger_Start, Hook_MobKillTrigger_Start, Orig_MobKillTrigger_Start
      );

      HOOK_SIGNATURE(
        "StatisticsService::AddPortalKill", Signatures::StatisticsService_AddPortalKill,
        Hook_StatisticsService_AddPortalKill, Orig_StatisticsService_AddPortalKill
      );

      HOOK_SIGNATURE(
        "StatisticsService::AddKillWithPortalProgress", Signatures::StatisticsService_AddKillWithPortalProgress,
        Hook_StatisticsService_AddKillWithPortalProgress, Orig_StatisticsService_AddKillWithPortalProgress
      );

      HOOK_SIGNATURE(
        "HourglassTalentConfig::get_Cost", Signatures::HourglassTalentConfig_get_Cost,
        Hook_HourglassTalentConfig_get_Cost, Orig_HourglassTalentConfig_get_Cost
      );

      HOOK_SIGNATURE(
        "HourglassSystemUI::BuildTalentTooltipContent", Signatures::HourglassSystemUI_BuildTalentTooltipContent,
        Hook_HourglassSystemUI_BuildTalentTooltipContent, Orig_HourglassSystemUI_BuildTalentTooltipContent
      );
    }

    void Uninitialize() { }
  }  // namespace HourglassBypass
}  // namespace Features
