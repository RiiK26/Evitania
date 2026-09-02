#include "GodMode.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace GodMode
  {
    void (*Orig_AttackReceiver_Recieve)(void* __this, void* attack, void* method_info);

    void Hook_AttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      bool isPlayer = *(bool*) ((uintptr_t) __this + 0x43);
      if (Menu::Config.bGodMode) {
        if (isPlayer) {
          // Infinite HP: nullify the attack damage by returning early (player takes no damage)
          return;
        }
        else {
          // High Attack: set the damage of the attack hitting the enemy to a massive amount
          if (attack) {
            *(float*) ((uintptr_t) attack + 0x10) = Menu::Config.fGodModeDamage;
          }
        }
      }
      Orig_AttackReceiver_Recieve(__this, attack, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "DamageSystem.Damagables.AttackReceiver", "Recieve", 1, Hook_AttackReceiver_Recieve, Orig_AttackReceiver_Recieve
      );
    }

    void Uninitialize() { }
  }  // namespace GodMode
}  // namespace Features
