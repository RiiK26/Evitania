#include "GodMode.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace GodMode
  {
    void (*Orig_AttackReceiver_Recieve)(void* __this, void* attack, void* method_info);

    void Hook_AttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      if (!Menu::Config.bGodMode) {
        return Orig_AttackReceiver_Recieve(__this, attack, method_info);
      }

      bool isPlayer = *(bool*) ((uintptr_t) __this + Offsets::Fields::AttackReceiver::isPlayer);
      if (isPlayer) {
        // Infinite HP: nullify the attack damage by returning early (player takes no damage)
        return;
      }

      // High Attack: set the damage of the attack hitting the enemy to a massive amount
      if (attack) {
        *(float*) ((uintptr_t) attack + Offsets::Fields::Attack::AttackDamage) = Menu::Config.fGodModeDamage;
      }

      Orig_AttackReceiver_Recieve(__this, attack, method_info);
    }

    void (*Orig_MovementControl_Move)(void* __this, float movespeed, void* method_info);

    void Hook_MovementControl_Move(void* __this, float movespeed, void* method_info)
    {
      if (Menu::Config.bGodMode) {
        // Check if the MovementControl belongs to a PlayerCharacter
        void* _view = *(void**) ((uintptr_t) __this + Offsets::Fields::MovementControl::_view);
        if (_view) {
          // Check if it's a network player
          void* networkPlayerSync = *(void**) ((uintptr_t) _view + Offsets::Fields::PlayerCharacter::networkPlayerSync);
          if (networkPlayerSync) {
            // _isNet
            bool _isNet = *(bool*) ((uintptr_t) networkPlayerSync + Offsets::Fields::NetworkPlayerSync::_isNet);
            if (!_isNet) {
              // It is the local player!
              movespeed *= Menu::Config.fGodModeSpeedMultiplier;
            }
          }
          else {
            // If no network sync is found, assume it's local player (e.g. singleplayer mode)
            movespeed *= Menu::Config.fGodModeSpeedMultiplier;
          }
        }
      }
      Orig_MovementControl_Move(__this, movespeed, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "DamageSystem.Damagables.AttackReceiver", "Recieve", 1, Hook_AttackReceiver_Recieve, Orig_AttackReceiver_Recieve
      );
      HOOK_SIGNATURE(
        "MovementControl::Move", Signatures::MovementControl_Move, Hook_MovementControl_Move, Orig_MovementControl_Move
      );
    }

    void Uninitialize() { }
  }  // namespace GodMode
}  // namespace Features
