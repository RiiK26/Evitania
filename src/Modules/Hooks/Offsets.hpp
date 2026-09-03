#pragma once
#include <cstdint>

namespace Offsets
{
  namespace CurrencyService
  {
    constexpr uintptr_t Subtract = 0x6EAB60;
  }  // namespace CurrencyService

  namespace Skill
  {
    // public void AddExperience(float amount) { }
    constexpr uintptr_t AddExperience = 0x77B210;
  }  // namespace Skill

  namespace FreeStore
  {
    constexpr uintptr_t SteamPurchaseService_InitiatePurchase  = 0x7675B0;
    constexpr uintptr_t MobilePurchaseService_InitiatePurchase = 0x749460;

    constexpr uintptr_t SteamPurchaseService_FindLot  = 0x767370;
    constexpr uintptr_t MobilePurchaseService_FindLot = 0x748D10;

    constexpr uintptr_t IAPRewarder_Reward = 0x7F2EB0;
  }  // namespace FreeStore

  namespace MovementControl
  {
    constexpr uintptr_t Move = 0x680530;
  }
  namespace Time
  {
    constexpr uintptr_t set_timeScale = 0x363C6B0;
  }
  namespace SpawnPortal
  {
    constexpr uintptr_t CurrentSpawnInterval = 0x89CB90;
  }
  namespace GatheringService
  {
    constexpr uintptr_t GetSpeed = 0x6F0790;
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
