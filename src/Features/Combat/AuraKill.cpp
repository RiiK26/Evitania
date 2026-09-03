#include "AuraKill.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
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

    void Hook_EnemyNpcController_TakeDamage(void* __this, float damage, void* method_info)
    {
      Orig_EnemyNpcController_TakeDamage(__this, damage, method_info);
    }

    std::unordered_map<void*, ULONGLONG> damageCooldowns;

    void Hook_EnemyNpcController_Update(void* __this, void* method_info)
    {
      if (Menu::Config.bAuraKill) {
        bool isAlive = *(bool*) ((uintptr_t) __this + 0x3C);
        if (isAlive) {
          ULONGLONG currentTick = GetTickCount64();
          if (currentTick - damageCooldowns[__this] > 1000) {
            Orig_EnemyNpcController_TakeDamage(__this, Menu::Config.fGodModeDamage, method_info);
            damageCooldowns[__this] = currentTick;
          }
        }
        else {
          // Cleanup if dead to prevent memory leaks from reused pointers
          damageCooldowns.erase(__this);
        }
      }
      Orig_EnemyNpcController_Update(__this, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "Enemy.EnemyNpcController", "TakeDamage", 1, Hook_EnemyNpcController_TakeDamage,
        Orig_EnemyNpcController_TakeDamage
      );
      HOOK_METHOD(
        "Enemy.EnemyNpcController", "Update", 0, Hook_EnemyNpcController_Update, Orig_EnemyNpcController_Update
      );
    }

    void Uninitialize() { }
  }  // namespace AuraKill
}  // namespace Features
