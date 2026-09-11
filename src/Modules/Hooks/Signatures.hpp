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

  // 0x802F50
  constexpr const char* TimeskipItem_CanUseImpl = "48 83 EC 28 80 3D CA ? ? ? ? 75 13 48 8D 0D 14 C4 3D 04";

  // 0x746CC0
  constexpr const char* HourglassService_Sand =
    "48 89 5C 24 08 57 48 83 EC 30 33 D2 0F 29 74 24 20 48 8B D9 E8 ? ? ? ? 80 3D 23 ? ? ? ?";

  // 0x740600
  constexpr const char* HourglassService_GrantReward = "48 89 5C 24 08 57 48 83 EC 40 80 3D F4 ? ? ? ? 48 8B FA";

  // 0x73E4E0
  constexpr const char* HourglassService_CostFactor =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D 21 ? ? ? ? 48 8B DA 0F 29 74 24 20";

  // 0x739B30
  constexpr const char* HourglassService_AccruePrestige = "40 53 48 83 EC 50 80 3D C5 ? ? ? ? 48 8B D9";

  // 0x825740
  constexpr const char* HourglassSimulator_Simulate = "40 55 56 48 81 EC ? ? ? ? 80 3D F7 ? ? ? ?";

  // 0x742890
  constexpr const char* HourglassService_PrestigeSpeedMult = "45 33 C9 45 33 C0 BA ? ? ? ? E9 ? ? ? ? 48 83 EC 38";

  // 0x742780
  constexpr const char* HourglassService_PrestigeCycleIncome =
    "45 33 C9 45 33 C0 BA ? ? ? ? E9 ? ? ? ? 40 53 48 83 EC 40";

  // 0x746090
  constexpr const char* HourglassService_UpgradeCost = "48 89 5C 24 08 56 48 83 EC 20 80 3D 79 ? ? ? ?";

  // 0x742000
  constexpr const char* HourglassService_LevelCost = "40 53 48 83 EC 20 48 8B 41 10 48 85 C0 74 2A";

  // 0x745870
  constexpr const char* HourglassService_TotalRate =
    "48 8B C4 48 89 58 10 48 89 70 18 57 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 D8 0F 29 78 C8 44 0F 29 40 B8 48 8B F1 80 3D 75 ? ? ? ?";

  // 0x742850
  constexpr const char* HourglassService_PrestigeRatePerSecond =
    "40 53 48 83 EC 30 33 D2 0F 29 74 24 20 48 8B D9 E8 ? ? ? ? 45 33 C9 45 33 C0";

  // 0x824250
  constexpr const char* HourglassTalentConfig_PrestigeCost = "F2 0F 10 41 48 C3 CC CC CC CC CC CC CC CC CC";

  // 0x856690
  constexpr const char* HourglassUpgradeItem_GetCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D C8 ? ? ? ? 8B FA 0F 29 74 24 40";

  // 0x824970
  constexpr const char* HourglassHardUUpgrade_GetCostAmount =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D D4 ? ? ? ? 8B FA 48 8B D9";

  // 0x73CDC0
  constexpr const char* HourglassService_BuyUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D 4D ? ? ? ? 48 8B FA 48 8B D9 75 1F 48 8D 0D C8 54 4D 04";

  // 0x73C220
  constexpr const char* HourglassService_BuyHardUpgrade = "40 55 56 48 83 EC 48 80 3D 02 ? ? ? ? 48 8B EA";

  // 0x73C8E0
  constexpr const char* HourglassService_BuyTalent = "48 89 5C 24 18 57 48 83 EC 40 80 3D 36 ? ? ? ?";

  // 0x73BF40
  constexpr const char* HourglassService_BuyGenerator =
    "48 89 6C 24 18 48 89 74 24 20 57 48 81 EC ? ? ? ? 80 3D BB ? ? ? ?";

  // 0x73CB60
  constexpr const char* HourglassService_BuyUpgradeBlock =
    "48 89 5C 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 41 56 48 83 EC 60 4C 8B F2";

}  // namespace Signatures
