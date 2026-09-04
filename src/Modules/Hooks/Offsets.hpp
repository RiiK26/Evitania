#pragma once
#include <cstdint>

namespace Offsets
{
  namespace CurrencyService
  {
    constexpr uintptr_t Subtract = 0x6EAFB0;
  }  // namespace CurrencyService

  namespace Skill
  {
    // public void AddExperience(float amount) { }
    constexpr uintptr_t AddExperience = 0x77B8F0;
  }  // namespace Skill

  namespace FreeStore
  {
    constexpr uintptr_t SteamPurchaseService_InitiatePurchase  = 0x767C90;
    constexpr uintptr_t MobilePurchaseService_InitiatePurchase = 0x749870;

    constexpr uintptr_t SteamPurchaseService_FindLot  = 0x767A50;
    constexpr uintptr_t MobilePurchaseService_FindLot = 0x749120;

    constexpr uintptr_t IAPRewarder_Reward = 0x7F3A20;
  }  // namespace FreeStore

  namespace MovementControl
  {
    constexpr uintptr_t Move = 0x680530;
  }
  namespace Time
  {
    constexpr uintptr_t set_timeScale = 0x363CE80;
  }
  namespace SpawnPortal
  {
    constexpr uintptr_t CurrentSpawnInterval = 0x89D700;
  }
  namespace GatheringService
  {
    constexpr uintptr_t GetSpeed = 0x6F0BE0;
  }

  namespace Fields
  {
    namespace EnemyNpcController
    {
      constexpr uintptr_t alive = 0x3C;
    }
    namespace AttackReceiver
    {
      constexpr uintptr_t isPlayer = 0x43;
    }
    namespace Attack
    {
      constexpr uintptr_t AttackDamage = 0x10;
    }
    namespace MovementControl
    {
      constexpr uintptr_t _view = 0x28;
    }
    namespace PlayerCharacter
    {
      constexpr uintptr_t networkPlayerSync = 0x88;
    }
    namespace NetworkPlayerSync
    {
      constexpr uintptr_t _isNet = 0x50;
    }
    namespace SteamPurchaseService
    {
      constexpr uintptr_t _rewarder = 0x20;
    }
  }  // namespace Fields
}  // namespace Offsets
