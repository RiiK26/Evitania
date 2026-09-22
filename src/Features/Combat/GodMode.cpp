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
    void (*Orig_RealmAttackReceiver_Recieve)(void* __this, void* attack, void* method_info);
    void (*Orig_EasterAttackReceiver_Recieve)(void* __this, void* attack, void* method_info);

    void (*Orig_MovementControl_Move)(void* __this, float movespeed, void* method_info);
    void (*Orig_PlayerCharacter_TakeDamage)(void* __this, float damage, void* method_info);

    bool ShouldNullifyDamage(void* __this)
    {
      if (!__this)
        return false;
      if (Offsets::isPlayer > 0) {
        bool isPlayer = *(bool*) ((uintptr_t) __this + Offsets::isPlayer);
        return (isPlayer && Menu::Config.bGodMode_Nullify);
      }
      return false;
    }

    void ApplyHighDamage(void* attack)
    {
      if (attack && Menu::Config.bGodMode_Damage) {
        if (Offsets::AttackDamage > 0) {
          *(float*) ((uintptr_t) attack + Offsets::AttackDamage) = Menu::Config.fGodModeDamage;
        }
      }
    }

    void Hook_MovementControl_Move(void* __this, float movespeed, void* method_info)
    {
      if (!__this)
        return Orig_MovementControl_Move(__this, movespeed, method_info);

      if (Menu::Config.bGodMode && Menu::Config.bGodMode_Speed) {
        if (Offsets::_view > 0 && Offsets::networkPlayerSync > 0 && Offsets::_isNet > 0) {
          // Check if the MovementControl belongs to a PlayerCharacter
          void* _view = *(void**) ((uintptr_t) __this + Offsets::_view);
          if (_view) {
            // Check if it's a network player
            void* networkPlayerSync = *(void**) ((uintptr_t) _view + Offsets::networkPlayerSync);
            if (networkPlayerSync) {
              // _isNet
              bool _isNet = *(bool*) ((uintptr_t) networkPlayerSync + Offsets::_isNet);
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

    void Hook_AttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      if (!Menu::Config.bGodMode)
        return Orig_AttackReceiver_Recieve(__this, attack, method_info);
      if (ShouldNullifyDamage(__this))
        return;
      ApplyHighDamage(attack);
      Orig_AttackReceiver_Recieve(__this, attack, method_info);
    }

    void Hook_RealmAttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      if (!Menu::Config.bGodMode)
        return Orig_RealmAttackReceiver_Recieve(__this, attack, method_info);
      if (ShouldNullifyDamage(__this))
        return;
      ApplyHighDamage(attack);
      Orig_RealmAttackReceiver_Recieve(__this, attack, method_info);
    }

    void Hook_EasterAttackReceiver_Recieve(void* __this, void* attack, void* method_info)
    {
      if (!Menu::Config.bGodMode)
        return Orig_EasterAttackReceiver_Recieve(__this, attack, method_info);
      if (ShouldNullifyDamage(__this))
        return;
      ApplyHighDamage(attack);
      Orig_EasterAttackReceiver_Recieve(__this, attack, method_info);
    }

    void Hook_PlayerCharacter_TakeDamage(void* __this, float damage, void* method_info)
    {
      if (!__this)
        return Orig_PlayerCharacter_TakeDamage(__this, damage, method_info);

      if (Menu::Config.bGodMode && Menu::Config.bGodMode_Nullify) {
        if (Offsets::networkPlayerSync > 0 && Offsets::_isNet > 0) {
          void* networkPlayerSync = *(void**) ((uintptr_t) __this + Offsets::networkPlayerSync);
          if (networkPlayerSync) {
            bool _isNet = *(bool*) ((uintptr_t) networkPlayerSync + Offsets::_isNet);
            if (!_isNet) {
              return;  // nullify damage for local player
            }
          }
          else {
            return;  // nullify damage for local player in singleplayer
          }
        }
      }
      Orig_PlayerCharacter_TakeDamage(__this, damage, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "AttackReceiver::Recieve", Signatures::AttackReceiver_Recieve, Hook_AttackReceiver_Recieve,
        Orig_AttackReceiver_Recieve
      );
      HOOK_SIGNATURE(
        "RealmAttackReceiver::Recieve", Signatures::RealmAttackReceiver_Recieve, Hook_RealmAttackReceiver_Recieve,
        Orig_RealmAttackReceiver_Recieve
      );
      HOOK_SIGNATURE(
        "EasterAttackReceiver::Recieve", Signatures::EasterAttackReceiver_Recieve, Hook_EasterAttackReceiver_Recieve,
        Orig_EasterAttackReceiver_Recieve
      );
      HOOK_SIGNATURE(
        "MovementControl::Move", Signatures::MovementControl_Move, Hook_MovementControl_Move, Orig_MovementControl_Move
      );
      HOOK_SIGNATURE(
        "PlayerCharacter::TakeDamage", Signatures::PlayerCharacter_TakeDamage, Hook_PlayerCharacter_TakeDamage,
        Orig_PlayerCharacter_TakeDamage
      );
    }

    void Uninitialize() { }
  }  // namespace GodMode
}  // namespace Features
