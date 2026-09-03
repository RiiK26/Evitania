#pragma once

namespace Signatures
{
  // 0x6EAB60
  constexpr const char* CurrencyService_Subtract = "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10";

  // 0x77B210
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30 8B 41 40 48 8B D9";

  // 0x7675B0
  constexpr const char* SteamPurchaseService_InitiatePurchase = "48 89 5C 24 20 57 48 83 EC 20 80 3D FD ? ? ? ?";

  // 0x749460
  constexpr const char* MobilePurchaseService_InitiatePurchase = "48 89 5C 24 10 57 48 83 EC 20 80 3D 34 ? ? ? ?";

  // 0x767370
  constexpr const char* SteamPurchaseService_FindLot = "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0";

  // 0x748D10
  constexpr const char* MobilePurchaseService_FindLot = "48 83 EC 28 48 8B 49 30 48 85 C9 74 0C 45 33 C0";

  // 0x7F2EB0
  constexpr const char* IAPRewarder_Reward = "40 53 56 57 41 56 48 83 EC 28 80 3D F2 ? ? ? ?";

  // 0x680530
  constexpr const char* MovementControl_Move = "40 53 48 81 EC ? ? ? ? 80 3D F3 ? ? ? ?";

  // 0x363C6B0
  constexpr const char* Time_timeScale = "48 83 EC 38 48 8B 05 D5 33 8E 01 0F 29 74 24 20";

  // 0x89CB90
  constexpr const char* SpawnPortal_CurrentSpawnInterval = "40 53 48 83 EC 30 80 3D 5D ? ? ? ? 48 8B D9";

  // 0x6F0790
  constexpr const char* GatheringService_GetSpeed = "48 83 EC 28 48 85 D2 74 42 8B 52 40 85 D2 74 18";

}  // namespace Signatures
