#pragma once

namespace Signatures
{
  // 0x6EAFB0
  constexpr const char* CurrencyService_Subtract = "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10 0F 28 F2";

  // 0x6EAE10
  constexpr const char* CurrencyService_Subtract_1 =
    "48 89 5C 24 10 56 48 81 EC ? ? ? ? 80 3D AE ? ? ? ? 48 8B DA 0F 29 74 24 70";

  // 0x77B8F0
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30 8B 41 40 48 8B D9";

  // 0x767C90
  constexpr const char* SteamPurchaseService_InitiatePurchase = "48 89 5C 24 20 57 48 83 EC 20 80 3D E6 ? ? ? ?";

  // 0x749870
  constexpr const char* MobilePurchaseService_InitiatePurchase =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ED ? ? ? ? 48 8B FA 48 8B D9 75 13 48 8D 0D D8 A2 4C 04";

  // 0x767A50
  constexpr const char* SteamPurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 08";

  // 0x749120
  constexpr const char* MobilePurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 30 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 83 EC 28";

  // 0x7F3A20
  constexpr const char* IAPRewarder_Reward = "40 53 56 57 41 56 48 83 EC 28 80 3D 4C ? ? ? ?";

  // 0x680530
  constexpr const char* MovementControl_Move =
    "40 53 48 81 EC ? ? ? ? 80 3D B3 ? ? ? ? 48 8B D9 0F 29 BC 24 E0 00 00 00";

  // 0x363CE80
  constexpr const char* Time_timeScale = "48 83 EC 38 48 8B 05 C5 3D 8E 01 0F 29 74 24 20";

  // 0x89D700
  constexpr const char* SpawnPortal_CurrentSpawnInterval = "40 53 48 83 EC 30 80 3D B7 ? ? ? ? 48 8B D9 48 89 7C 24 40";

  // 0x6F0BE0
  constexpr const char* GatheringService_GetSpeed = "48 83 EC 28 48 85 D2 74 42 8B 52 40 85 D2 74 18";

}  // namespace Signatures
