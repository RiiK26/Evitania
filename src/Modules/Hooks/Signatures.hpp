#pragma once

namespace Signatures
{
  // 0x93CA00
  constexpr const char* EnemyNpcController_TakeDamage = "40 53 48 83 EC 20 80 79 3C 00 48 8B D9 74 4C";

  // 0x93CCC0
  constexpr const char* EnemyNpcController_Update = "40 53 48 83 EC 40 80 79 20 00 48 8B D9 74 4D";

  // 0x939340
  constexpr const char* BossBase_Update = "80 79 3C 00 74 1D 48 8B 89 20 01 00 00 48 85 C9";

  // 0x94B350
  constexpr const char* WorldElite_Update =
    "40 53 48 83 EC 50 0F 29 74 24 40 33 D2 0F 29 7C 24 30 48 8B D9 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 45 33 C0 48 8D "
    "4C 24 20 48 8B D0 E8 ? ? ? ? F3 0F 10 35 ? ? ? ?";

  // 0x99C920
  constexpr const char* AttackReceiver_Recieve = "40 53 56 48 83 EC 78 80 3D ? ? ? ? 00 48 8B F2";

  // 0x8974E0
  constexpr const char* RealmAttackReceiver_Recieve = "48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2";

  // 0x9834D0
  constexpr const char* EasterAttackReceiver_Recieve = "48 89 74 24 18 41 56 48 83 EC 60 80 3D ? ? ? ? 00";

  // 0x99C750
  constexpr const char* AttackReceiver_Awake =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 4B 38 48 89 43 38 48 8B D0 E8 ? ? ? ? 4C 8B 53 38 33 D2";

  // 0x72DCA0
  constexpr const char* CurrencyService_Subtract = "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10 0F 28 F2";

  // 0x72DB00
  constexpr const char* CurrencyService_Subtract_1 =
    "48 89 5C 24 10 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 70";

  // 0x7C5BC0
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30 8B 41 40 48 8B D9";

  // 0x7AFB60
  constexpr const char* SteamPurchaseService_InitiatePurchase =
    "48 89 5C 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 73";

  // 0x7ACE70
  constexpr const char* MobilePurchaseService_InitiatePurchase =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 80 7B 3C 00";

  // 0x7AF920
  constexpr const char* SteamPurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 08";

  // 0x83AC90
  constexpr const char* IAPRewarder_Reward = "40 53 56 57 41 56 48 83 EC 28 80 3D ? ? ? ? 00 45 0F B6 F1 41 0F B6 F0";

  // 0x6A3440
  constexpr const char* MovementControl_Move =
    "40 53 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B D9 0F 29 BC 24 E0 00 00 00";

  // 0x37BFB00
  constexpr const char* Time_set_timeScale =
    "48 83 EC 38 48 8B 05 ? ? ? ? 0F 29 74 24 20 0F 28 F0 48 85 C0 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? "
    "0F 28 C6 0F 28 74 24 20 48 83 C4 38 48 FF E0 CC CC CC CC CC CC 40 53 48 83 EC 20 80 3D ? ? ? ? 00";

  // 0x861060
  constexpr const char* TimeskipItem_get_CanUseImpl =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 48 8B 08 48 85 C9 74 0B 33 D2 48 83 C4 28 "
    "E9 ? ? ? ? E8 ? ? ? ? CC CC 40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ?";

  // 0x783920
  constexpr const char* HourglassService_CostFactor =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 20 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 4C 8B 03";

  // 0x78B4D0
  constexpr const char* HourglassService_UpgradeCost =
    "48 89 5C 24 08 56 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B F1 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 48 C7 44 24 38 ? ? ? ? 48 85 DB 74 51 48 8B 4E 78 48 85 C9 74 56 4C 8B 0D ? ? ? ?";

  // 0x787440
  constexpr const char* HourglassService_LevelCost = "40 53 48 83 EC 20 48 8B 41 10 48 85 C0 74 2A";

  // 0x892E80
  constexpr const char* HourglassUpgradeItem_GetCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 0F 29 74 24 40";

  // 0x88DFA0
  constexpr const char* HourglassHardUUpgrade_GetCostAmount =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 8B FA 48 8B D9";

  // 0x782200
  constexpr const char* HourglassService_BuyUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? C6 05 ? ? ? ? 01 33 D2 48 8B CB E8 ? ? ? ? 45 33 C0 48 8B D7";

  // 0x781660
  constexpr const char* HourglassService_BuyHardUpgrade =
    "40 55 56 48 83 EC 48 80 3D ? ? ? ? 00 48 8B EA 48 8B F1 75 37 48 8D 0D ? ? ? ?";

  // 0x781D20
  constexpr const char* HourglassService_BuyTalent =
    "48 89 5C 24 18 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 0F 57 C0 48 89 74 24 50";

  // 0x781380
  constexpr const char* HourglassService_BuyGenerator =
    "48 89 6C 24 18 48 89 74 24 20 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B E8 48 8B F2";

  // 0x781FA0
  constexpr const char* HourglassService_BuyUpgradeBlock =
    "48 89 5C 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 41 56 48 83 EC 60 4C 8B F2";

  // 0x7B7030
  constexpr const char* AntiCheatService_Initialize =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F1 75 5B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ?";

  // 0x7B74F0
  constexpr const char* AntiCheatService_OnSpeedHackDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 77";

  // 0x7B73A0
  constexpr const char* AntiCheatService_OnObscuredCheatingDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 7A";

  // 0x7B6F30
  constexpr const char* AntiCheatService_Handle = "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 "
                                                  "13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7B 28 00 74 23";

  // 0x7B6B00
  constexpr const char* AntiCheatService_Apply =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 8B 03";

  // 0x7B7640
  constexpr const char* AntiCheatService_ReportCheatToAnalytics =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA 48 8B F1 0F 85 ? ? ? ?";

  // 0x79A8E0
  constexpr const char* CurioGachaService_RollRarity =
    "48 89 54 24 10 48 89 4C 24 08 53 56 57 41 56 41 57 48 83 EC 70 0F 29 74 24 60";

  // 0x79D8F0
  constexpr const char* CurioPowerService_GetLevelUpCost =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 41 8B F0 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? "
    "? ? ? C6 05 ? ? ? ? 01 48 8B 4B 18";

  // 0x7A7850
  constexpr const char* EngineerService_TryGetUpgradeCost =
    "40 53 55 56 57 41 56 48 83 EC 30 80 3D ? ? ? ? 00 4D 8B F1 49 8B F8 8B F2";

  // 0x8B4700
  constexpr const char* EngineerUpgradeConfig_GetPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 89 5E 10 48 8B 0D ? ? ? ?";

  // 0x6FEF10
  constexpr const char* MarketLotScriptableObject_CurrentPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 48 8D 4E 18 48 89 6C 24 30";

  // 0x6FF300
  constexpr const char* MarketLot_GetCurrentPrice =
    "48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F9 0F 29 74 24 20 0F 28 F1 75 7F";

  // 0x7C4190
  constexpr const char* PlayerCharacter_TakeDamage = "48 83 EC 38 80 3D ? ? ? ? 00 0F 29 74 24 20 0F 28 F1 75 13 48 8D "
                                                     "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ?";

  // 0x781120
  constexpr const char* HourglassService_BuyCoinCdUpgrade =
    "40 53 48 83 EC 30 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 43 10 48 89 7C 24 40 0F 29 74 24 20 48 85 C0 0F 84 ? ? ? ? 48 8B B8 98 00 00 00 33 D2 48 8B CB E8 "
    "? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 50 68 48 85 D2 0F 84 ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 8B 52 10 45 33 C0";

  // 0x781250
  constexpr const char* HourglassService_BuyCoinRewardUpgrade =
    "40 53 48 83 EC 30 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 43 10 48 89 7C 24 40 0F 29 74 24 20 48 85 C0 0F 84 ? ? ? ? 48 8B B8 98 00 00 00 33 D2 48 8B CB E8 "
    "? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 50 68 48 85 D2 0F 84 ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 8B 52 14 45 33 C0";

  // 0x781900
  constexpr const char* HourglassService_BuyRiftWorker =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D "
    "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 33 D2 48 8B CB E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 90 80 00 00 00";

  // 0x781A50
  constexpr const char* HourglassService_BuyShopItem =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA";

  // 0x7829F0
  constexpr const char* HourglassService_CoinCdCost =
    "40 53 48 83 EC 20 48 8B 41 10 48 85 C0 74 34 48 8B 98 98 00 00 00 33 D2 E8 ? ? ? ? 48 85 C0 74 21 48 8B 50 68 48 "
    "85 D2 74 18 48 85 DB 74 13 8B 52 10 45 33 C0 48 8B CB 48 83 C4 20 5B E9 ? ? ? ? E8 ? ? ? ? CC CC CC CC CC CC CC "
    "CC 48 83 EC 28 33 D2";

  // 0x783200
  constexpr const char* HourglassService_CoinRewardCost =
    "40 53 48 83 EC 20 48 8B 41 10 48 85 C0 74 34 48 8B 98 98 00 00 00 33 D2 E8 ? ? ? ? 48 85 C0 74 21 48 8B 50 68 48 "
    "85 D2 74 18 48 85 DB 74 13 8B 52 14 45 33 C0 48 8B CB 48 83 C4 20 5B E9 ? ? ? ? E8 ? ? ? ? CC CC CC CC CC CC CC "
    "CC 40 53 48 83 EC 20";

  // 0x789D40
  constexpr const char* HourglassService_ShopNextCost = "40 53 48 83 EC 20 48 8B DA 48 85 D2 74 1B 45 33 C0";

  // 0x88E6E0
  constexpr const char* HourglassLevelConfig_SandCost =
    "48 89 5C 24 08 57 48 83 EC 60 80 3D ? ? ? ? 00 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ?";

  // 0x88E5E0
  constexpr const char* HourglassLevelConfig_GoldCost = "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 8B DA 48 8B F9 "
                                                        "75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 41 B1 01";

}  // namespace Signatures
