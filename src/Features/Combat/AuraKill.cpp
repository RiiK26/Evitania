#include "AuraKill.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

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

    void Hook_EnemyNpcController_Update(void* __this, void* method_info)
    {
      if (Menu::Config.bAuraKill) {
        bool isAlive = *(bool*) ((uintptr_t) __this + 0x3C);
        if (isAlive) {
          Orig_EnemyNpcController_TakeDamage(__this, 999999.0f, method_info);
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
