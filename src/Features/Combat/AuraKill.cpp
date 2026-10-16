#include "AuraKill.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>
#include <unordered_map>

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

#include <windows.h>

namespace Features
{
  namespace AuraKill
  {
    void (*Orig_EnemyNpcController_TakeDamage)(void* __this, float damage, void* method_info);
    void (*Orig_EnemyNpcController_Update)(void* __this, void* method_info);
    void (*Orig_BossBase_Update)(void* __this, void* method_info);
    void (*Orig_WorldElite_Update)(void* __this, void* method_info);
    void (*Orig_YellowDragonBoss_Update)(void* __this, void* method_info);

    void Hook_EnemyNpcController_TakeDamage(void* __this, float damage, void* method_info)
    {
      Orig_EnemyNpcController_TakeDamage(__this, damage, method_info);
    }

    std::unordered_map<void*, ULONGLONG> damageCooldowns;

    void ApplyAuraKill(void* __this, void* method_info)
    {
      if (!Menu::Config.bAuraKill || !__this)
        return;

      if (Offsets::alive > 0) {
        bool isAlive = *(bool*) ((uintptr_t) __this + Offsets::alive);
        if (isAlive) {
          ULONGLONG currentTick = GetTickCount64();
          auto      it          = damageCooldowns.find(__this);

          if (it == damageCooldowns.end() || currentTick - it->second > 1000) {
            Orig_EnemyNpcController_TakeDamage(__this, Menu::Config.fGodModeDamage, method_info);
            damageCooldowns[__this] = currentTick;

            if (damageCooldowns.size() > 256) {
              for (auto cleanIt = damageCooldowns.begin(); cleanIt != damageCooldowns.end();) {
                if (currentTick - cleanIt->second > 3000) {
                  cleanIt = damageCooldowns.erase(cleanIt);
                }
                else {
                  ++cleanIt;
                }
              }
            }
          }
        }
      }
    }

    void Hook_EnemyNpcController_Update(void* __this, void* method_info)
    {
      ApplyAuraKill(__this, method_info);
      Orig_EnemyNpcController_Update(__this, method_info);
    }

    void Hook_BossBase_Update(void* __this, void* method_info)
    {
      ApplyAuraKill(__this, method_info);
      Orig_BossBase_Update(__this, method_info);
    }

    void Hook_WorldElite_Update(void* __this, void* method_info)
    {
      ApplyAuraKill(__this, method_info);
      Orig_WorldElite_Update(__this, method_info);
    }

    void Hook_YellowDragonBoss_Update(void* __this, void* method_info)
    {
      ApplyAuraKill(__this, method_info);
      Orig_YellowDragonBoss_Update(__this, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "EnemyNpcController::TakeDamage", Signatures::EnemyNpcController_TakeDamage, Hook_EnemyNpcController_TakeDamage,
        Orig_EnemyNpcController_TakeDamage
      );
      HOOK_SIGNATURE(
        "EnemyNpcController::Update", Signatures::EnemyNpcController_Update, Hook_EnemyNpcController_Update,
        Orig_EnemyNpcController_Update
      );
      HOOK_SIGNATURE("BossBase::Update", Signatures::BossBase_Update, Hook_BossBase_Update, Orig_BossBase_Update);
      HOOK_SIGNATURE(
        "WorldElite::Update", Signatures::WorldElite_Update, Hook_WorldElite_Update, Orig_WorldElite_Update
      );
      HOOK_SIGNATURE(
        "YellowDragonBoss::Update", Signatures::YellowDragonBoss_Update, Hook_YellowDragonBoss_Update,
        Orig_YellowDragonBoss_Update
      );
    }

    void Uninitialize() { damageCooldowns.clear(); }
  }  // namespace AuraKill
}  // namespace Features
