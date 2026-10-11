#include "AuraKill.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>
#include <unordered_map>
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
    void (*Orig_BlueDragonBoss_Update)(void* __this, void* method_info);

    void Hook_EnemyNpcController_TakeDamage(void* __this, float damage, void* method_info)
    {
      Orig_EnemyNpcController_TakeDamage(__this, damage, method_info);
    }

    std::unordered_map<void*, ULONGLONG> damageCooldowns;
    ULONGLONG                            lastCleanupTick = 0;

    void ApplyAuraKill(void* __this, void* method_info)
    {
      if (!Menu::Config.bAuraKill || !__this || Offsets::alive <= 0) {
        if (!Menu::Config.bAuraKill && !damageCooldowns.empty()) {
          damageCooldowns.clear();
        }
        return;
      }

      bool isAlive = *(bool*) ((uintptr_t) __this + Offsets::alive);
      if (!isAlive) {
        return;
      }

      ULONGLONG currentTick = GetTickCount64();

      // Periodically prune stale entries every 5 seconds to avoid expensive per-frame lookups for dead entities
      if (currentTick - lastCleanupTick > 5000) {
        lastCleanupTick = currentTick;
        for (auto it = damageCooldowns.begin(); it != damageCooldowns.end();) {
          if (currentTick - it->second > 5000) {
            it = damageCooldowns.erase(it);
          }
          else {
            ++it;
          }
        }
      }

      auto it = damageCooldowns.find(__this);
      if (it == damageCooldowns.end()) {
        damageCooldowns.emplace(__this, currentTick);
        Orig_EnemyNpcController_TakeDamage(__this, Menu::Config.fGodModeDamage, method_info);
      }
      else if (currentTick - it->second >= 1000) {
        it->second = currentTick;
        Orig_EnemyNpcController_TakeDamage(__this, Menu::Config.fGodModeDamage, method_info);
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

    void Hook_BlueDragonBoss_Update(void* __this, void* method_info)
    {
      ApplyAuraKill(__this, method_info);
      Orig_BlueDragonBoss_Update(__this, method_info);
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
      HOOK_SIGNATURE(
        "BlueDragonBoss::Update", Signatures::BlueDragonBoss_Update, Hook_BlueDragonBoss_Update,
        Orig_BlueDragonBoss_Update
      );
    }

    void Uninitialize() { damageCooldowns.clear(); }
  }  // namespace AuraKill
}  // namespace Features
