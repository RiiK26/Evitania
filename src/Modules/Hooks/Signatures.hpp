#pragma once

namespace Signatures
{
  // 0xAA5A90
  constexpr const char* ACTkDetectorBase_InvokeCheatingDetectedEvent = "48 8B 41 40 48 85 C0 74 0C 48 8B 50 28";

  // 0x2332E00
  constexpr const char* ACTkDetectorBase_OnCheatingDetected = "40 53 48 83 EC 20 48 8B D9 C6 41 48 01";

  // 0x8637F0
  constexpr const char* AntiCheatService_Apply =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 8B 07 48 8D 54 24 48";

  // 0x863BC0
  constexpr const char* AntiCheatService_Handle = "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 "
                                                  "13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7B 30 00";

  // 0x863CC0
  constexpr const char* AntiCheatService_Initialize =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F1 75 5B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ?";

  // 0x863EC0
  constexpr const char* AntiCheatService_OnDataLoaded =
    "48 89 5C 24 10 48 89 4C 24 08 57 48 81 EC ? ? ? ? 48 8B F9 80 3D ? ? ? ? 00 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 "
    "05 ? ? ? ? 01 0F 57 C0 0F 11 44 24 60";

  // 0x864040
  constexpr const char* AntiCheatService_OnObscuredCheatingDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 7A 33 D2";

  // 0x864190
  constexpr const char* AntiCheatService_OnSpeedHackDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 77 33 D2";

  // 0x8642E0
  constexpr const char* AntiCheatService_ReportCheatToAnalytics =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA 48 8B F1 0F 85 ? ? ? ?";

  // 0x864580
  constexpr const char* AntiCheatService_SetFlag = "48 83 EC 28 85 D2 74 3D";

  // 0xA9F650
  constexpr const char* AttackReceiver_Awake =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 4B 38 48 89 43 38 48 8B D0 E8 ? ? ? ? 4C 8B 53 38";

  // 0xA9F820
  constexpr const char* AttackReceiver_Recieve =
    "48 89 74 24 18 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2 48 8B F1 75 5B 48 8D 0D ? ? ? ?";

  // 0x6E3B60
  constexpr const char* BaseStorageService_Remove =
    "48 89 4C 24 08 53 56 57 41 54 41 56 41 57 48 83 EC 68 4D 8B F0 48 8B DA";

  // 0x6E4510
  constexpr const char* BaseStorageService_TryRemove =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 49 8B D8 48 8B FA 48 8B F1 75 1F 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7E 20 00";

  // 0xA03510
  constexpr const char* BonfireFuelBurner_get_Fuel = "48 83 EC 28 48 8B 41 18 48 85 C0 74 0A F3 0F 10 40 14";

  // 0x9067E0
  constexpr const char* BonfireFuelBurner_get_MaxFuel =
    "F3 0F 10 41 34 C3 CC CC CC CC CC CC CC CC CC CC F3 0F 10 41 28";

  // 0x78D800
  constexpr const char* BonfireService_TryIgnite =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 43 10 48 85 C0 0F 84 ? ? ? ? 48 8B 40 50";

  // 0x78DFB0
  constexpr const char* BonfireService_get_CanIgnite =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 43 10 48 85 "
    "C0 74 54 48 8B 40 50";

  // 0x78E060
  constexpr const char* BonfireService_get_Lit =
    "48 83 EC 28 48 8B 41 10 48 85 C0 74 1E 48 8B 40 50 48 85 C0 74 15 48 8B 80 80 00 00 00";

  // 0xA387F0
  constexpr const char* BossBase_Update = "80 79 3C 00 74 1D 48 8B 89 38 01 00 00";

  // 0x866F30
  constexpr const char* CurioGachaService_RollRarity =
    "48 89 54 24 10 48 89 4C 24 08 53 56 57 41 56 41 57 48 83 EC 70 0F 29 74 24 60";

  // 0x86A020
  constexpr const char* CurioPowerService_GetLevelUpCost =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 41 8B F0 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? "
    "? ? ? C6 05 ? ? ? ? 01 48 8B 4B 18";

  // 0x7A75C0
  constexpr const char* CurrencyService_Subtract =
    "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10 0F 28 F2 48 85 C9";

  // 0x7A7460
  constexpr const char* CurrencyService_Subtract_1 =
    "48 89 5C 24 10 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 70";

  // 0xAA32F0
  constexpr const char* EasterAttackReceiver_Recieve =
    "48 89 74 24 18 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2 48 8B F1 75 67 48 8D 0D ? ? ? ?";

  // 0xA3BE90
  constexpr const char* EnemyNpcController_TakeDamage =
    "40 53 48 83 EC 20 80 79 3C 00 48 8B D9 74 4C F3 0F 10 91 90 00 00 00";

  // 0xA3C150
  constexpr const char* EnemyNpcController_Update = "40 53 48 83 EC 40 80 79 20 00";

  // 0x84FA10
  constexpr const char* EngineerService_TryGetUpgradeCost =
    "40 55 56 57 41 56 48 83 EC 38 80 3D ? ? ? ? 00 4D 8B F1 49 8B F0 8B EA 48 8B F9";

  // 0x9BFA00
  constexpr const char* EngineerUpgradeConfig_GetPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 89 5E 10";

  // 0xB3BDB0
  constexpr const char* EnhancementTreeUI_Buy =
    "40 53 55 56 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B E9 0F 85 ? ? ? ?";

  // 0x99EEE0
  constexpr const char* HourglassHardUUpgrade_GetCostAmount =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 8B FA";

  // 0x99F5C0
  constexpr const char* HourglassLevelConfig_SandCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 45 33 C9";

  // 0x82D2D0
  constexpr const char* HourglassService_BuyGenerator =
    "48 89 6C 24 18 48 89 74 24 20 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B E8";

  // 0x82D630
  constexpr const char* HourglassService_BuyHardUpgrade =
    "40 55 56 48 83 EC 48 80 3D ? ? ? ? 00 48 8B EA 48 8B F1 75 37 48 8D 0D ? ? ? ?";

  // 0x82D8D0
  constexpr const char* HourglassService_BuyShopItem =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA";

  // 0x82DE20
  constexpr const char* HourglassService_BuyTalent = "48 89 5C 24 20 56 48 83 EC 60 80 3D ? ? ? ? 00";

  // 0x82E650
  constexpr const char* HourglassService_BuyUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? C6 05 ? ? ? ? 01 33 D2 48 8B CB E8 ? ? ? ? 45 33 C0";

  // 0x82E3F0
  constexpr const char* HourglassService_BuyUpgradeBlock =
    "48 89 5C 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 41 56 48 83 EC 60 4C 8B F2 48 8B F1";

  // 0x82F750
  constexpr const char* HourglassService_CostFactor =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 20 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 4C 8B 03 45 33 C9";

  // 0x835620
  constexpr const char* HourglassService_LevelCost = "40 53 48 83 EC 20 48 8B 81 80 00 00 00 48 85 C0 74 2A";

  // 0x83AFB0
  constexpr const char* HourglassService_ShopNextCost = "40 53 48 83 EC 20 48 8B DA 48 85 D2 74 1B";

  // 0x83CD80
  constexpr const char* HourglassService_UpgradeCost = "48 89 5C 24 08 56 48 83 EC 20 80 3D ? ? ? ? 00";

  // 0xA534C0
  constexpr const char* HourglassSystemUI_BuildTalentTooltipContent =
    "40 53 56 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B F2 48 8B D9";

  // 0x99DF50
  constexpr const char* HourglassTalentConfig_get_Cost = "F2 0F 10 41 48 C3 CC CC";

  // 0xAFEA30
  constexpr const char* HourglassTalentNodeView_SetState =
    "40 53 55 56 57 48 83 EC 28 80 3D ? ? ? ? 00 0F B6 FA 41 0F B6 F1";

  // 0x9A6840
  constexpr const char* HourglassUpgradeItem_GetCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 0F 29 74 24 40";

  // 0x8D6A80
  constexpr const char* IAPRewarder_Reward =
    "48 89 5C 24 10 48 89 74 24 18 57 41 56 41 57 48 83 EC 20 80 3D ? ? ? ? 00 45 0F B6 F1";

  // 0x656980
  constexpr const char* ItemDetail_get_Amount = "48 8B 41 18 C3 CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 44 24 08";

  // 0xB36660
  constexpr const char* MapUi_UpdatePlayerLocation =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 8B 08 01 00 00";

  // 0x77DD10
  constexpr const char* MarketLotScriptableObject_CurrentPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 48 8D 4E 18";

  // 0x77E120
  constexpr const char* MarketLot_GetCurrentPrice =
    "48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F9 0F 29 74 24 20 0F 28 F1 75 7F 48 8D 0D ? ? ? ?";

  // 0x6CBDE0
  constexpr const char* MetaModel_get_IsCheater = "80 79 38 00 75 17 80 79 39 00";

  // 0x8582E0
  constexpr const char* MobilePurchaseService_InitiatePurchase =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 80 7B 4C 00 48 C7 44 24 30 ? ? ? ? 75 78";

  // 0x6CC790
  constexpr const char* MovementControl_Move =
    "40 53 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B D9 0F 29 BC 24 E0 00 00 00";

  // 0x682290
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 49 8B E8 48 8B DA 4C 8B F1";

  // 0x682360
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected_1 =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 41 0F B6 E8";

  // 0x66ADC0
  constexpr const char* ObscuredFilePrefs_OnDataFromAnotherDeviceDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 40 08";

  // 0x66AE30
  constexpr const char* ObscuredFilePrefs_OnNotGenuineDataDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 00";

  // 0x873FD0
  constexpr const char* PlayerCharacter_TakeDamage = "48 83 EC 38 80 3D ? ? ? ? 00 0F 29 74 24 20 0F 28 F1 75 13 48 8D "
                                                     "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ?";

  // 0x7F9E70
  constexpr const char* PlayerDataStorageService_FireAccountMismatchAnalytics =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 40 80 3D ? ? ? ? 00 49 8B D8 48 8B F2 48 8B F9 75 43";

  // 0x7C6650
  constexpr const char* PositionerService_TryChangeScene =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30 80 3D ? ? ? ? 00 41 0F B6 F8 48 8B F2 48 8B D9 75 1F";

  // 0x9AA8E0
  constexpr const char* RealmAttackReceiver_Recieve =
    "48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2 48 8B F1 75 67 48 8D 0D ? ? ? ?";

  // 0xA2FCB0
  constexpr const char* SacrificeUI_Buy =
    "40 56 57 48 83 EC 48 80 3D ? ? ? ? 00 48 8B F2 48 8B F9 75 7F 48 8D 0D ? ? ? ?";

  // 0xA30CA0
  constexpr const char* SacrificeUI_TryRollSacrificeHalfCost =
    "48 83 EC 38 33 D2 0F 29 74 24 20 B9 ? ? ? ? E8 ? ? ? ? 33 C9 0F 28 F0 E8 ? ? ? ? 0F 2F F0 0F 28 74 24 20 0F 97 C0 "
    "48 83 C4 38 C3 CC 40 53 48 83 EC 20";

  // 0x781F40
  constexpr const char* ShopSpendingService_CanSpend =
    "48 89 5C 24 10 48 89 74 24 18 57 48 81 EC ? ? ? ? 0F 29 74 24 70 41 8B F0 48 8B FA";

  // 0x785B80
  constexpr const char* ShopSpendingService_Spend =
    "48 83 EC 28 48 8B 49 18 48 85 C9 74 0C 45 33 C9 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 40 53";

  // 0x875A30
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30";

  // 0x69B650
  constexpr const char* SpeedHackDetector_IsTicksCheated =
    "40 53 55 56 57 48 83 EC 38 80 3D ? ? ? ? 00 49 8B F1 0F 29 74 24 20";

  // 0x69B6F0
  constexpr const char* SpeedHackDetector_IsTimeScaleCheated = "40 53 48 83 EC 30 80 79 7D 00";

  // 0x85B3D0
  constexpr const char* SteamPurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 08";

  // 0x85B610
  constexpr const char* SteamPurchaseService_InitiatePurchase =
    "48 89 5C 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 73 48 8D 0D ? ? ? ?";

  // 0x69E190
  constexpr const char* TimeCheatingDetector_ReportCheckResult =
    "41 56 48 83 EC 20 80 3D ? ? ? ? 00 4C 8B F1 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 49 8B 86 D8 00 00 "
    "00";

  // 0x39E0790
  constexpr const char* Time_set_timeScale =
    "48 83 EC 38 48 8B 05 ? ? ? ? 0F 29 74 24 20 0F 28 F0 48 85 C0 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? "
    "0F 28 C6 0F 28 74 24 20 48 83 C4 38 48 FF E0 CC CC CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9";

  // 0x8FF280
  constexpr const char* TimeskipItem_get_CanUseImpl =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 48 8B 08 48 85 C9 74 0B 33 D2 48 83 C4 28 "
    "E9 ? ? ? ? E8 ? ? ? ? CC CC 40 53 48 83 EC 20 8B 41 18";

  // 0xA48630
  constexpr const char* WorldElite_Update =
    "40 53 48 83 EC 50 0F 29 74 24 40 33 D2 0F 29 7C 24 30 48 8B D9 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 45 33 C0 48 8D "
    "4C 24 20 48 8B D0 E8 ? ? ? ? F3 0F 10 35 ? ? ? ?";

  // 0xA4CF00
  constexpr const char* YellowDragonBoss_Update = "40 53 48 83 EC 50 80 79 3C 00";

}  // namespace Signatures
