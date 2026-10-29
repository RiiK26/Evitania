#pragma once

namespace Signatures
{
  // 0xB3F2A0
  constexpr const char* ACTkDetectorBase_InvokeCheatingDetectedEvent = "48 8B 41 40 48 85 C0 74 0C 48 8B 50 28";

  // 0x2403C90
  constexpr const char* ACTkDetectorBase_OnCheatingDetected = "40 53 48 83 EC 20 48 8B D9 C6 41 48 01";

  // 0x8A23B0
  constexpr const char* AntiCheatService_Apply =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 8B 07 48 8D 54 24 48";

  // 0x8A2780
  constexpr const char* AntiCheatService_Handle = "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 "
                                                  "13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7B 30 00";

  // 0x8A2880
  constexpr const char* AntiCheatService_Initialize =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F1 75 5B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ?";

  // 0x8A2A80
  constexpr const char* AntiCheatService_OnDataLoaded =
    "48 89 5C 24 10 48 89 4C 24 08 57 48 81 EC ? ? ? ? 48 8B F9 80 3D ? ? ? ? 00 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 "
    "05 ? ? ? ? 01 0F 57 C0 0F 11 44 24 60";

  // 0x8A2C00
  constexpr const char* AntiCheatService_OnObscuredCheatingDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 7A 33 D2";

  // 0x8A2D50
  constexpr const char* AntiCheatService_OnSpeedHackDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 77 33 D2";

  // 0x8A2EA0
  constexpr const char* AntiCheatService_ReportCheatToAnalytics =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA 48 8B F1 0F 85 ? ? ? ?";

  // 0x8A3140
  constexpr const char* AntiCheatService_SetFlag = "48 83 EC 28 85 D2 74 3D";

  // 0xB370A0
  constexpr const char* AttackReceiver_Awake =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 4B 38 48 89 43 38 48 8B D0 E8 ? ? ? ? 4C 8B 53 38";

  // 0xB37270
  constexpr const char* AttackReceiver_Recieve = "48 89 74 24 18 41 56 48 83 EC 60 80 3D ? ? ? ? 00";

  // 0xB37CF0
  constexpr const char* Attack_ctor = "48 89 5C 24 08 57 48 83 EC 40 0F 29 74 24 30 48 8B DA 0F 29 7C 24 20 33 D2 0F "
                                      "28 FB 0F 28 F2 48 8B F9 E8 ? ? ? ? 48 8D 4F 18";

  // 0x6E51B0
  constexpr const char* BaseStorageService_Remove =
    "48 89 4C 24 08 53 56 57 41 54 41 56 41 57 48 83 EC 68 4D 8B F0 48 8B DA";

  // 0x6E5B60
  constexpr const char* BaseStorageService_TryRemove =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 49 8B F8 48 8B DA 48 8B F1 75 1F 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7E 20 00";

  // 0xABA0D0
  constexpr const char* BonfireFuelBurner_get_Fuel = "48 83 EC 28 48 8B 41 18 48 85 C0 74 0A F3 0F 10 40 14";

  // 0x96DE90
  constexpr const char* BonfireFuelBurner_get_MaxFuel =
    "F3 0F 10 41 34 C3 CC CC CC CC CC CC CC CC CC CC F3 0F 10 41 2C";

  // 0x7C0FB0
  constexpr const char* BonfireService_TryIgnite =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 43 10 48 85 C0 0F 84 ? ? ? ?";

  // 0x7C1760
  constexpr const char* BonfireService_get_CanIgnite =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 43 10 48 85 "
    "C0 74 54 48 8B 40 50";

  // 0x7C1810
  constexpr const char* BonfireService_get_Lit =
    "48 83 EC 28 48 8B 41 10 48 85 C0 74 1E 48 8B 40 50 48 85 C0 74 15 48 8B 80 80 00 00 00";

  // 0xAEDD40
  constexpr const char* BossBase_Update = "80 79 3C 00 74 1D 48 8B 89 38 01 00 00";

  // 0x682E60
  constexpr const char* CombatOfflineRewardsCalculator_SetCurrentSpawner =
    "48 89 51 38 48 83 C1 38 E9 ? ? ? ? CC CC CC 48 89 5C 24 18";

  // 0x8A4280
  constexpr const char* CurioGachaService_RollRarity =
    "48 89 54 24 10 48 89 4C 24 08 53 56 57 41 56 41 57 48 83 EC 70 0F 29 74 24 60";

  // 0x8A73F0
  constexpr const char* CurioPowerService_GetLevelUpCost =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 41 8B F0 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? "
    "? ? ? C6 05 ? ? ? ? 01 48 8B 4B 18";

  // 0x7DC4E0
  constexpr const char* CurrencyService_Subtract =
    "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10 0F 28 F2 48 85 C9";

  // 0x7DC380
  constexpr const char* CurrencyService_Subtract_1 =
    "48 89 5C 24 10 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 70";

  // 0xB3CA70
  constexpr const char* EasterAttackReceiver_Recieve =
    "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 48 8B F2 4C 8B F1";

  // 0x6470D0
  constexpr const char* EnemyCard_get_UniqueName = "48 8B 41 20 C3 CC CC CC CC CC CC CC CC CC CC CC 48 89 51 20";

  // 0xAD4D40
  constexpr const char* EnemyNpcController_TakeDamage =
    "40 53 48 83 EC 20 80 79 3C 00 48 8B D9 74 4C F3 0F 10 91 90 00 00 00";

  // 0xAD5000
  constexpr const char* EnemyNpcController_Update = "40 53 48 83 EC 40 80 79 20 00";

  // 0x8AC8E0
  constexpr const char* EnemySpawnerService_get_EnemyCard =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 4B 30 48 85 "
    "C9 74 13 4C 8B 05 ? ? ? ? 33 D2 48 83 C4 20 5B E9 ? ? ? ? E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC 0F B6 41 4D";

  // 0x88D6E0
  constexpr const char* EngineerService_TryGetUpgradeCost =
    "40 55 56 57 41 56 48 83 EC 38 80 3D ? ? ? ? 00 4D 8B F1 49 8B F0 8B EA 48 8B F9";

  // 0xBC1090
  constexpr const char* EngineerSystemUI_TryBuyUpgrade =
    "48 89 5C 24 10 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 80 BF 98 01 00 00 00";

  // 0xA38A70
  constexpr const char* EngineerUpgradeConfig_GetPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 89 5E 10";

  // 0x6FD980
  constexpr const char* EnhancementTreeUI_Buy =
    "40 53 55 56 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B E9 0F 85 ? ? ? ?";

  // 0x6FE180
  constexpr const char* EnhancementTreeUI_Select = "48 89 5C 24 10 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 "
                                                   "75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 0F 29 74 24 30";

  // 0xAC3890
  constexpr const char* EntityCardDatabase_ctor =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B C8 48 8B D8 E8 ? ? ? ? 48 8D 4F 18 48 89 "
    "5F 18 48 8B D3 E8 ? ? ? ? 33 D2 48 8B CF 48 8B 5C 24 30 48 83 C4 20 5F E9 ? ? ? ? CC CC CC CC CC CC CC CC CC 48 "
    "89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 13 48 8D 0D ? ? ? ?";

  // 0xA278B0
  constexpr const char* HourglassHardUUpgrade_GetCostAmount =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 8B FA";

  // 0xA27F90
  constexpr const char* HourglassLevelConfig_SandCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 45 33 C9";

  // 0x868010
  constexpr const char* HourglassService_AddResource =
    "48 89 5C 24 10 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B DA 0F 29 7C 24 30";

  // 0x86A8F0
  constexpr const char* HourglassService_BuyGenerator =
    "48 89 6C 24 18 48 89 74 24 20 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B E8";

  // 0x86AC50
  constexpr const char* HourglassService_BuyHardUpgrade =
    "40 55 56 48 83 EC 48 80 3D ? ? ? ? 00 48 8B EA 48 8B F1 75 37 48 8D 0D ? ? ? ?";

  // 0x86AEF0
  constexpr const char* HourglassService_BuyShopItem =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA";

  // 0x86B440
  constexpr const char* HourglassService_BuyTalent = "48 89 5C 24 20 56 48 83 EC 60 80 3D ? ? ? ? 00";

  // 0x86BC70
  constexpr const char* HourglassService_BuyUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? C6 05 ? ? ? ? 01 33 D2 48 8B CB E8 ? ? ? ? 45 33 C0";

  // 0x86BA10
  constexpr const char* HourglassService_BuyUpgradeBlock =
    "48 89 5C 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 41 56 48 83 EC 60 4C 8B F2 48 8B F1";

  // 0x86CC90
  constexpr const char* HourglassService_CostFactor =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 20 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 4C 8B 03 45 33 C9";

  // 0x872CE0
  constexpr const char* HourglassService_LevelCost = "40 53 48 83 EC 20 48 8B 81 80 00 00 00 48 85 C0 74 2A";

  // 0x878910
  constexpr const char* HourglassService_ShopNextCost = "40 53 48 83 EC 20 48 8B DA 48 85 D2 74 1B";

  // 0x87AA90
  constexpr const char* HourglassService_UpgradeCost = "48 89 5C 24 08 56 48 83 EC 20 80 3D ? ? ? ? 00";

  // 0xB65450
  constexpr const char* HourglassSystemUI_BuildTalentTooltipContent =
    "40 53 56 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B F2 48 8B D9";

  // 0xA268C0
  constexpr const char* HourglassTalentConfig_get_Cost = "F2 0F 10 41 48 C3 CC CC";

  // 0xBA9480
  constexpr const char* HourglassTalentNodeView_SetState =
    "40 53 55 56 57 48 83 EC 28 80 3D ? ? ? ? 00 0F B6 FA 41 0F B6 F1";

  // 0xA2F1C0
  constexpr const char* HourglassUpgradeItem_GetCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 0F 29 74 24 40";

  // 0x92C7E0
  constexpr const char* IAPRewarder_Reward =
    "48 89 5C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 80 3D ? ? ? ? 00 45 0F B6 F1";

  // 0x659980
  constexpr const char* ItemDetail_get_Amount = "48 8B 41 18 C3 CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 44 24 08";

  // 0xBE5B40
  constexpr const char* MapUi_UpdatePlayerLocation =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 8B 18 01 00 00";

  // 0x7B1310
  constexpr const char* MarketLotScriptableObject_CurrentPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 48 8D 4E 18";

  // 0x7B1720
  constexpr const char* MarketLot_GetCurrentPrice =
    "48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F9 0F 29 74 24 20 0F 28 F1 75 7F 48 8D 0D ? ? ? ?";

  // 0x6CE5B0
  constexpr const char* MetaModel_get_IsCheater = "80 79 38 00 75 17 80 79 39 00";

  // 0xAE36A0
  constexpr const char* MobKillTrigger_Apply =
    "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F2 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 48 63 43 20";

  // 0xAE38B0
  constexpr const char* MobKillTrigger_CurrentKills =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 4B 28 48 89 "
    "7C 24 30 48 85 C9 74 42 48 8B 05 ? ? ? ?";

  // 0xAE3970
  constexpr const char* MobKillTrigger_OnDisable =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F1 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 88 A0 00 00 00 48 8B 79 10 48 8B 0D ? ? ? ? E8 ? ? ? ? "
    "4C 8B 05 ? ? ? ? 45 33 C9 48 8B D6 48 8B C8 48 8B D8 E8 ? ? ? ? 45 33 C0 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B D8 48 "
    "85 C0 75 16 48 8B 05 ? ? ? ? 33 D2 48 8B 88 A0 00 00 00 48 89 59 10 EB 47 48 8B 3D ? ? ? ? 48 8B CB 48 8B D7 E8 ? "
    "? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 91 A0 00 00 00 48 8B CB 48 89 42 10 48 8B 3D ? ? ? ? 48 8B D7 "
    "E8 ? ? ? ? 48 8B D0 48 85 C0 74 79 48 8B 05 ? ? ? ?";

  // 0xAE3B10
  constexpr const char* MobKillTrigger_OnEnable =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F1 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 88 A0 00 00 00 48 8B 79 10 48 8B 0D ? ? ? ? E8 ? ? ? ? "
    "4C 8B 05 ? ? ? ? 45 33 C9 48 8B D6 48 8B C8 48 8B D8 E8 ? ? ? ? 45 33 C0 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B D8 48 "
    "85 C0 75 16 48 8B 05 ? ? ? ? 33 D2 48 8B 88 A0 00 00 00 48 89 59 10 EB 47 48 8B 3D ? ? ? ? 48 8B CB 48 8B D7 E8 ? "
    "? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 91 A0 00 00 00 48 8B CB 48 89 42 10 48 8B 3D ? ? ? ? 48 8B D7 "
    "E8 ? ? ? ? 48 8B D0 48 85 C0 74 76 48 8B 05 ? ? ? ?";

  // 0xAE3930
  constexpr const char* MobKillTrigger_Start = "40 53 48 83 EC 20 83 79 20 00 48 8B D9 7E 21 48 83 79 28 00";

  // 0x896880
  constexpr const char* MobilePurchaseService_InitiatePurchase =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 80 7B 4C 00 48 C7 44 24 30 ? ? ? ? 75 78";

  // 0x6EF1F0
  constexpr const char* MovementControl_Move =
    "40 53 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B D9 0F 29 BC 24 E0 00 00 00";

  // 0x685290
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 49 8B E8 48 8B DA 4C 8B F1";

  // 0x685360
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected_1 =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 41 0F B6 E8";

  // 0x66DDC0
  constexpr const char* ObscuredFilePrefs_OnDataFromAnotherDeviceDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 40 08";

  // 0x66DE30
  constexpr const char* ObscuredFilePrefs_OnNotGenuineDataDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 00";

  // 0x87D7B0
  constexpr const char* OfflineProgressService_CanUseTimeskip =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? 48 C7 44 24 30 ? ? ? ?";

  // 0x87E470
  constexpr const char* OfflineProgressService_CreateActivitySnapshot =
    "48 89 5C 24 10 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 4F 48 8D 0D ? ? ? ?";

  // 0x87E810
  constexpr const char* OfflineProgressService_CurrentActivitySnapshot =
    "48 89 5C 24 08 57 48 83 EC 30 48 8B F9 0F 29 74 24 20";

  // 0x6AA0F0
  constexpr const char* OfflineProgressService_GetCombatCalculator =
    "48 8B 41 48 C3 CC CC CC CC CC CC CC CC CC CC CC 48 8B 41 58";

  // 0x880340
  constexpr const char* OfflineProgressService_SetCurrentActivity =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 5B 48 8D 0D ? ? ? ?";

  // 0x881AE0
  constexpr const char* OfflineProgressService_WriteClearedActivity =
    "48 89 5C 24 08 57 48 83 EC 20 48 8B 41 20 48 8B D9 48 85 C0 0F 84 ? ? ? ? 48 8B 40 50";

  // 0x8B30E0
  constexpr const char* PlayerCharacter_TakeDamage = "48 83 EC 38 80 3D ? ? ? ? 00 0F 29 74 24 20 0F 28 F1 75 13 48 8D "
                                                     "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ?";

  // 0x8B3310
  constexpr const char* PlayerCharacter_ctor =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 C7 87 B8 00 00 00 ? ? ? ?";

  // 0x84D3F0
  constexpr const char* PlayerDataStorageService_FireAccountMismatchAnalytics =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 40 80 3D ? ? ? ? 00 49 8B D8 48 8B F2 48 8B F9 75 43";

  // 0x7FBEB0
  constexpr const char* PositionerService_TryChangeScene =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30 80 3D ? ? ? ? 00 41 0F B6 F8 48 8B F2 48 8B D9 75 1F";

  // 0xA11D20
  constexpr const char* RealmAttackReceiver_Recieve =
    "48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2 48 8B F1 75 73 48 8D 0D ? ? ? ?";

  // 0xACAB40
  constexpr const char* SacrificeUI_Buy =
    "40 56 57 48 83 EC 48 80 3D ? ? ? ? 00 48 8B F2 48 8B F9 75 7F 48 8D 0D ? ? ? ?";

  // 0xACB340
  constexpr const char* SacrificeUI_Select =
    "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B E9";

  // 0xACBB30
  constexpr const char* SacrificeUI_TryRollSacrificeHalfCost =
    "48 83 EC 38 33 D2 0F 29 74 24 20 B9 ? ? ? ? E8 ? ? ? ? 33 C9 0F 28 F0 E8 ? ? ? ? 0F 2F F0 0F 28 74 24 20 0F 97 C0 "
    "48 83 C4 38 C3 CC 40 53 48 83 EC 20";

  // 0x7B56A0
  constexpr const char* ShopSpendingService_CanSpend =
    "48 89 5C 24 10 48 89 74 24 18 57 48 81 EC ? ? ? ? 0F 29 74 24 70 41 8B F0 48 8B FA";

  // 0x7B59A0
  constexpr const char* ShopSpendingService_Spend = "48 8B C4 48 89 58 18 48 89 50 10 48 89 48 08 56 57 41 54 41 56 41 "
                                                    "57 48 81 EC ? ? ? ? 0F 29 70 C8 0F 29 78 B8 41 8B D8 4C 8B FA";

  // 0x7B6DC0
  constexpr const char* Shop_TryBuy =
    "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B F1 0F 85 ? ? ? ?";

  // 0x7B72C0
  constexpr const char* Shop_UpdateDiscount = "40 53 48 83 EC 40 80 79 50 00";

  // 0x8C8230
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30";

  // 0x69E650
  constexpr const char* SpeedHackDetector_IsTicksCheated =
    "40 53 55 56 57 48 83 EC 38 80 3D ? ? ? ? 00 49 8B F1 0F 29 74 24 20";

  // 0x69E6F0
  constexpr const char* SpeedHackDetector_IsTimeScaleCheated = "40 53 48 83 EC 30 80 79 7D 00";

  // 0x802A40
  constexpr const char* StatisticsService_AddKillWithPortalProgress =
    "40 53 55 56 57 48 83 EC 38 49 8B F9 48 C7 44 24 20 ? ? ? ?";

  // 0x802DB0
  constexpr const char* StatisticsService_AddPortalKill = "40 55 57 41 54 41 56 41 57 48 83 EC 40 80 3D ? ? ? ? 00";

  // 0x899B10
  constexpr const char* SteamPurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 08";

  // 0x899D50
  constexpr const char* SteamPurchaseService_InitiatePurchase =
    "48 89 5C 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 73 48 8D 0D ? ? ? ?";

  // 0x6A1190
  constexpr const char* TimeCheatingDetector_ReportCheckResult =
    "41 56 48 83 EC 20 80 3D ? ? ? ? 00 4C 8B F1 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 49 8B 86 D8 00 00 "
    "00";

  // 0x3A9C4C0
  constexpr const char* Time_set_timeScale =
    "48 83 EC 38 48 8B 05 ? ? ? ? 0F 29 74 24 20 0F 28 F0 48 85 C0 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? "
    "0F 28 C6 0F 28 74 24 20 48 83 C4 38 48 FF E0 CC CC CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9";

  // 0x939B70
  constexpr const char* TimeskipItem_get_CanUseImpl =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 48 8B 08 48 85 C9 74 0B 33 D2 48 83 C4 28 "
    "E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 10";

  // 0xAEB840
  constexpr const char* WorldElite_Update =
    "40 53 48 83 EC 50 0F 29 74 24 40 33 D2 0F 29 7C 24 30 48 8B D9 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 45 33 C0 48 8D "
    "4C 24 20 48 8B D0 E8 ? ? ? ? F3 0F 10 35 ? ? ? ?";

  // 0xB01BC0
  constexpr const char* YellowDragonBoss_Update = "40 53 48 83 EC 50 80 79 3C 00";

}  // namespace Signatures
