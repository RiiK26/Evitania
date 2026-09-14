#include "GodMode.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"

#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace GodMode
  {
    void (*Orig_AttackReceiver_Recieve)(void* __this, void* attack, void* method_info);

    static int isPlayerOffset     = -1;
    static int attackDamageOffset = -1;

    void Hook_AttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      if (!Menu::Config.bGodMode) {
        return Orig_AttackReceiver_Recieve(__this, attack, method_info);
      }

      if (isPlayerOffset == -1) {
        isPlayerOffset = IL2CPP::Class::Utils::GetFieldOffset("DamageSystem.Damagables.AttackReceiver", "isPlayer");
      }

      if (isPlayerOffset > 0) {
        bool isPlayer = *(bool*) ((uintptr_t) __this + isPlayerOffset);
        if (isPlayer && Menu::Config.bGodMode_Nullify) {
          // Infinite HP: nullify the attack damage by returning early (player takes no damage)
          return;
        }
      }

      // High Attack: set the damage of the attack hitting the enemy to a massive amount
      if (attack && Menu::Config.bGodMode_Damage) {
        if (attackDamageOffset == -1) {
          attackDamageOffset = IL2CPP::Class::Utils::GetFieldOffset("DamageSystem.Attacks.Attack", "AttackDamage");
        }
        if (attackDamageOffset > 0) {
          *(float*) ((uintptr_t) attack + attackDamageOffset) = Menu::Config.fGodModeDamage;
        }
      }

      Orig_AttackReceiver_Recieve(__this, attack, method_info);
    }

    void (*Orig_MovementControl_Move)(void* __this, float movespeed, void* method_info);

    static int viewOffset              = -1;
    static int networkPlayerSyncOffset = -1;
    static int isNetOffset             = -1;

    void Hook_MovementControl_Move(void* __this, float movespeed, void* method_info)
    {
      if (Menu::Config.bGodMode && Menu::Config.bGodMode_Speed) {
        if (viewOffset == -1)
          viewOffset = IL2CPP::Class::Utils::GetFieldOffset("MovementControl", "_view");
        if (networkPlayerSyncOffset == -1)
          networkPlayerSyncOffset = IL2CPP::Class::Utils::GetFieldOffset("Player.PlayerCharacter", "networkPlayerSync");
        if (isNetOffset == -1)
          isNetOffset = IL2CPP::Class::Utils::GetFieldOffset("Net.NetworkPlayerSync", "_isNet");

        if (viewOffset > 0 && networkPlayerSyncOffset > 0 && isNetOffset > 0) {
          // Check if the MovementControl belongs to a PlayerCharacter
          void* _view = *(void**) ((uintptr_t) __this + viewOffset);
          if (_view) {
            // Check if it's a network player
            void* networkPlayerSync = *(void**) ((uintptr_t) _view + networkPlayerSyncOffset);
            if (networkPlayerSync) {
              // _isNet
              bool _isNet = *(bool*) ((uintptr_t) networkPlayerSync + isNetOffset);
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
      }
      Orig_MovementControl_Move(__this, movespeed, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "AttackReceiver::Recieve", Signatures::AttackReceiver_Recieve, Hook_AttackReceiver_Recieve,
        Orig_AttackReceiver_Recieve
      );
      HOOK_SIGNATURE(
        "MovementControl::Move", Signatures::MovementControl_Move, Hook_MovementControl_Move, Orig_MovementControl_Move
      );
    }

    void Uninitialize() { }
  }  // namespace GodMode
}  // namespace Features
