/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

namespace Signatures
{
  // 0x25EBB50
  constexpr const char* ACTkDetectorBase_OnCheatingDetected = "40 53 48 83 EC 20 48 8B D9 C6 41 48 01";

  // 0x8F4AE0
  constexpr const char* AntiCheatService_Apply =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9";

  // 0x8F4EC0
  constexpr const char* AntiCheatService_Handle = "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 "
                                                  "13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7B 30 00";

  // 0x8F4FC0
  constexpr const char* AntiCheatService_Initialize =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F1 75 5B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ?";

  // 0x8F51B0
  constexpr const char* AntiCheatService_OnDataLoaded =
    "48 89 5C 24 10 48 89 4C 24 08 57 48 81 EC ? ? ? ? 48 8B F9 80 3D ? ? ? ? 00 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 "
    "05 ? ? ? ? 01 0F 57 C0 0F 11 44 24 60";

  // 0x8F5330
  constexpr const char* AntiCheatService_OnObscuredCheatingDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 7A 33 D2";

  // 0x8F5480
  constexpr const char* AntiCheatService_OnSpeedHackDetected =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 75 08 48 8B C8 "
    "E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 0D ? ? ? ? 48 8B 18 83 B9 E4 00 00 00 00 "
    "75 05 E8 ? ? ? ? 45 33 C0 33 D2 48 8B CB E8 ? ? ? ? 84 C0 74 58 48 8B 05 ? ? ? ? 48 8B 40 20 F6 80 35 01 00 00 01 "
    "75 08 48 8B C8 E8 ? ? ? ? 48 8B 80 A8 00 00 00 48 8B 48 10 48 8B 81 A0 00 00 00 48 8B 08 48 85 C9 74 77 33 D2";

  // 0x8F55D0
  constexpr const char* AntiCheatService_ReportCheatToAnalytics =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA 48 8B F1";

  // 0x8F5870
  constexpr const char* AntiCheatService_SetFlag = "48 83 EC 28 85 D2 74 3D";

  // 0xC4EA30
  constexpr const char* AttackReceiver_Awake =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 15 ? ? ? ? 48 8B CB E8 ? ? ? ? 48 8D 4B 38 48 89 43 38 48 8B D0 E8 ? ? ? ? 4C 8B 53 38";

  // 0xC4EC00
  constexpr const char* AttackReceiver_Recieve = "48 89 74 24 18 41 56 48 83 EC 60 80 3D ? ? ? ? 00";

  // 0xC4F680
  constexpr const char* Attack_ctor = "48 89 5C 24 08 57 48 83 EC 40 0F 29 74 24 30 48 8B DA 0F 29 7C 24 20 33 D2 0F "
                                      "28 FB 0F 28 F2 48 8B F9 E8 ? ? ? ? 48 8D 4F 18";

  // 0x703010
  constexpr const char* BaseStorageService_Remove =
    "48 89 4C 24 08 53 56 57 41 54 41 56 41 57 48 83 EC 68 4D 8B F0 48 8B DA";

  // 0x7039C0
  constexpr const char* BaseStorageService_TryRemove =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 49 8B D8 48 8B FA 48 8B F1 75 1F 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 7E 20 00";

  // 0xBB6E50
  constexpr const char* BonfireFuelBurner_get_Fuel = "48 83 EC 28 48 8B 41 20 48 85 C0 74 0A F3 0F 10 40 14";

  // 0x7EC2F0
  constexpr const char* BonfireFuelBurner_get_MaxFuel =
    "F3 0F 10 41 3C C3 CC CC CC CC CC CC CC CC CC CC F3 0F 11 49 3C";

  // 0x7FD710
  constexpr const char* BonfireService_TryIgnite =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 43 18 48 89 "
    "7C 24 30 48 85 C0 0F 84 ? ? ? ? 48 8B 40 40";

  // 0x7FDEC0
  constexpr const char* BonfireService_get_CanIgnite = "40 53 48 83 EC 20 48 8B 41 18 48 85 C0 74 46 48 8B 40 40";

  // 0x7FDF20
  constexpr const char* BonfireService_get_Lit =
    "48 83 EC 28 48 8B 41 18 48 85 C0 74 1E 48 8B 40 40 48 85 C0 74 15 48 8B 80 80 00 00 00";

  // 0xBEBB80
  constexpr const char* BossBase_Update = "80 79 3C 00 74 1D 48 8B 89 48 01 00 00";

  // 0x6A0E60
  constexpr const char* CombatOfflineRewardsCalculator_SetCurrentSpawner =
    "48 89 51 38 48 83 C1 38 E9 ? ? ? ? CC CC CC 48 89 5C 24 18";

  // 0x8F8D50
  constexpr const char* CurioGachaService_RollRarity =
    "48 89 54 24 10 48 89 4C 24 08 53 56 57 41 56 41 57 48 83 EC 70 0F 29 74 24 60";

  // 0x8FC5A0
  constexpr const char* CurioPowerService_GetLevelUpCost =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 41 8B F0 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? "
    "? ? ? C6 05 ? ? ? ? 01 48 8B 4B 18";

  // 0x819B20
  constexpr const char* CurrencyService_Subtract =
    "40 53 48 83 EC 30 48 8B D9 0F 29 74 24 20 48 8B 49 10 0F 28 F2 48 85 C9";

  // 0x8199C0
  constexpr const char* CurrencyService_Subtract_1 =
    "48 89 5C 24 10 56 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 70";

  // 0xC51590
  constexpr const char* EasterAttackReceiver_Recieve =
    "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 48 8B F2 4C 8B F1";

  // 0x6650D0
  constexpr const char* EnemyCard_get_UniqueName = "48 8B 41 20 C3 CC CC CC CC CC CC CC CC CC CC CC 48 89 51 20";

  // 0xBEF960
  constexpr const char* EnemyNpcController_TakeDamage =
    "40 53 48 83 EC 20 80 79 3C 00 48 8B D9 74 4C F3 0F 10 91 90 00 00 00";

  // 0xBEFC20
  constexpr const char* EnemyNpcController_Update = "40 53 48 83 EC 40 80 79 20 00";

  // 0x901D00
  constexpr const char* EnemySpawnerService_get_EnemyCard =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 4B 30 48 85 "
    "C9 74 13 4C 8B 05 ? ? ? ? 33 D2 48 83 C4 20 5B E9 ? ? ? ? E8 ? ? ? ? CC CC CC CC CC CC CC CC CC CC 0F B6 41 69";

  // 0x8DDFA0
  constexpr const char* EngineerService_TryGetUpgradeCost =
    "40 55 56 57 41 56 48 83 EC 38 80 3D ? ? ? ? 00 4D 8B F1 49 8B F0 8B EA 48 8B F9";

  // 0xCAA6E0
  constexpr const char* EngineerSystemUI_TryBuyUpgrade =
    "48 89 5C 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 "
    "80 BF C8 01 00 00 00";

  // 0xB3BA90
  constexpr const char* EngineerUpgradeConfig_GetPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 89 5E 10";

  // 0x71B300
  constexpr const char* EnhancementTreeUI_Buy =
    "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B F1 75 5B 48 8D 0D ? ? ? ?";

  // 0x71BD10
  constexpr const char* EnhancementTreeUI_Select = "48 89 5C 24 10 48 89 6C 24 18 57 48 83 EC 40 0F 29 74 24 30";

  // 0xBBDD50
  constexpr const char* EntityCardDatabase_ctor =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 48 8B 15 ? ? ? ? 48 8B C8 48 8B D8 E8 ? ? ? ? 48 8D 4F 18 48 89 "
    "5F 18 48 8B D3 E8 ? ? ? ? 33 D2 48 8B CF 48 8B 5C 24 30 48 83 C4 20 5F E9 ? ? ? ? CC CC CC CC CC CC CC CC CC 48 "
    "89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 13 48 8D 0D ? ? ? ?";

  // 0xAFD850
  constexpr const char* HourglassCalc_GeometricMaxAffordable = "40 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B F8";

  // 0xB28C10
  constexpr const char* HourglassHardUUpgrade_GetCostAmount =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 8B FA";

  // 0xB292F0
  constexpr const char* HourglassLevelConfig_SandCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 45 33 C9";

  // 0xB293D0
  constexpr const char* HourglassLevelConfig_TableCost =
    "48 8B C4 44 89 40 18 48 89 50 10 48 89 48 08 53 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 "
    "78 A8 44 0F 29 40 98 44 0F 29 48 88 45 8B F8";

  // 0x8BB5A0
  constexpr const char* HourglassService_AddResource =
    "48 89 5C 24 10 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B DA 0F 29 7C 24 30";

  // 0x8BDD30
  constexpr const char* HourglassService_BulkCost = "48 89 5C 24 08 48 89 74 24 10 57 48 81 EC ? ? ? ? 0F 57 C0 33 C0";

  // 0x8BDE80
  constexpr const char* HourglassService_BuyGenerator =
    "48 89 6C 24 18 48 89 74 24 20 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 49 8B E8";

  // 0x8BE1E0
  constexpr const char* HourglassService_BuyHardUpgrade =
    "40 55 56 48 83 EC 48 80 3D ? ? ? ? 00 48 8B EA 48 8B F1 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 3D ? ? ? ? 00";

  // 0x8BE470
  constexpr const char* HourglassService_BuyShopItem =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B FA";

  // 0x8BE9B0
  constexpr const char* HourglassService_BuyTalent = "40 53 56 48 83 EC 68 80 3D ? ? ? ? 00 48 8B F2 48 8B D9";

  // 0x8BEDE0
  constexpr const char* HourglassService_BuyTimeEcho =
    "40 57 48 83 EC 50 80 3D ? ? ? ? 00 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D "
    "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 15 ? ? ? ? 45 33 C0 48 89 5C 24 60";

  // 0x8BF1E0
  constexpr const char* HourglassService_BuyUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? C6 05 ? ? ? ? 01 33 D2 48 8B CB E8 ? ? ? ? 45 33 C0";

  // 0x8BEF80
  constexpr const char* HourglassService_BuyUpgradeBlock =
    "48 89 5C 24 18 48 89 54 24 10 48 89 4C 24 08 56 57 41 56 48 83 EC 60 4C 8B F2 48 8B F1";

  // 0x8BF290
  constexpr const char* HourglassService_CanAffordHardUpgrade =
    "48 89 5C 24 20 56 48 83 EC 40 80 3D ? ? ? ? 00 48 8B DA 48 8B F1 75 13 48 8D 0D ? ? ? ?";

  // 0x8BF720
  constexpr const char* HourglassService_CanBuyTalent = "40 53 57 48 83 EC 48 45 33 C0";

  // 0x8BFAB0
  constexpr const char* HourglassService_CanReroll =
    "40 53 48 83 EC 30 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 15 ? ? ? ? "
    "45 33 C0 48 8B CB E8 ? ? ? ? 84 C0 75 06 48 83 C4 30 5B C3 48 8B 83 88 00 00 00";

  // 0x8C01E0
  constexpr const char* HourglassService_CostFactor =
    "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 0F 29 74 24 20 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 4C 8B 03 45 33 C9";

  // 0x8C3560
  constexpr const char* HourglassService_FindPortal =
    "48 83 EC 28 48 8B 89 88 00 00 00 48 85 C9 74 1F 48 8B 89 A8 00 00 00";

  // 0x8C35A0
  constexpr const char* HourglassService_FindTalent =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 48 C7 44 24 38 ? ? ? ? 48 85 DB 74 39 48 83 BF 08 01 00 00 00";

  // 0x8C3620
  constexpr const char* HourglassService_FindUpgrade =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 48 C7 44 24 38 ? ? ? ? 48 85 DB 74 34 48 8B 8F E8 00 00 00";

  // 0x8C3F40
  constexpr const char* HourglassService_GrantReward =
    "48 89 5C 24 18 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B FA 0F 29 74 24 30";

  // 0x8C4270
  constexpr const char* HourglassService_HardUpgradeCost =
    "48 89 5C 24 18 48 89 74 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 49 8B D8 48 8B FA 48 8B F1 75 13 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? C6 05 ? ? ? ? 01 80 3D ? ? ? ? 00";

  // 0x8C5C60
  constexpr const char* HourglassService_IsTalentUnlocked =
    "48 89 5C 24 08 48 89 74 24 18 57 48 83 EC 40 48 8B FA 48 8B F1 80 3D ? ? ? ? 00 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? "
    "C6 05 ? ? ? ? 01 48 C7 44 24 58 ? ? ? ? 80 3D ? ? ? ? 00";

  // 0x8C6230
  constexpr const char* HourglassService_LevelCost = "40 53 48 83 EC 20 48 8B 81 88 00 00 00 48 85 C0 74 2A";

  // 0x8C6400
  constexpr const char* HourglassService_MaxAffordable =
    "48 89 5C 24 08 57 48 81 EC ? ? ? ? 0F 57 C0 33 C0 38 05 ? ? ? ? 48 8B FA 48 8B D9 48 89 44 24 70";

  // 0x8C7480
  constexpr const char* HourglassService_OnWindowClose =
    "40 53 48 83 EC 40 80 3D ? ? ? ? 00 48 8B D9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D "
    "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 93 B0 00 00 00";

  // 0x8CA9E0
  constexpr const char* HourglassService_RiftFreeWorkers =
    "40 53 48 83 EC 30 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 80 3D ? ? ? ? 00 "
    "75 1F 48 8D 0D ? ? ? ?";

  // 0x8CADD0
  constexpr const char* HourglassService_RiftRewardPerHour =
    "40 53 55 56 57 48 83 EC 28 48 8B 81 88 00 00 00 48 8B F2 48 8B D9 48 85 C0 74 6D 48 8B 88 A8 00 00 00 48 85 C9 74 "
    "55 45 33 C0 E8 ? ? ? ? 48 8B E8 48 85 C0 74 45 48 8B 8B 88 00 00 00 48 85 C9 74 45 48 8B B9 A8 00 00 00 45 33 C0 "
    "48 8B CB 48 8B D6 E8 ? ? ? ? 48 85 C0 74 2B 48 85 FF 74 26 44 8B 40 10 45 33 C9 48 8B D5 48 8B CF 48 83 C4 28 5F "
    "5E 5D 5B E9 ? ? ? ? 0F 57 C0 48 83 C4 28 5F 5E 5D 5B C3 E8 ? ? ? ? CC CC CC 48 89 74 24 10";

  // 0x8CAF70
  constexpr const char* HourglassService_RiftUsedWorkers =
    "48 89 5C 24 08 57 48 81 EC ? ? ? ? 48 8B D9 80 3D ? ? ? ? 00 75 43 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? "
    "E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 "
    "0F 57 C0 33 C0 0F 11 44 24 50 0F 11 44 24 60 48 89 44 24 70 33 D2";

  // 0x8CBE40
  constexpr const char* HourglassService_ShopNextCost = "40 53 48 83 EC 20 48 8B DA 48 85 D2 74 1B";

  // 0x8CCCA0
  constexpr const char* HourglassService_TimeEchoBatchCost = "40 53 48 83 EC 50 48 8B 81 88 00 00 00";

  // 0x8CDB90
  constexpr const char* HourglassService_TreeBalance =
    "48 89 5C 24 08 57 48 83 EC 40 0F 29 74 24 30 48 8B FA 48 8B D9 48 85 D2";

  // 0x8CDD10
  constexpr const char* HourglassService_TreeOf =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 48 85 DB 74 26 48 8B 8F 10 01 00 00";

  // 0x8CDFC0
  constexpr const char* HourglassService_UpgradeCost = "48 89 5C 24 08 56 48 83 EC 20 80 3D ? ? ? ? 00";

  // 0x8CF420
  constexpr const char* HourglassService_get_RiftTotalWorkers =
    "40 53 48 83 EC 30 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? "
    "? ? ? 01 48 8B 83 88 00 00 00";

  // 0xB982D0
  constexpr const char* HourglassSystemUI_BuildTalentTooltipContent =
    "40 53 56 57 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B F2 48 8B D9";

  // 0xAFFFE0
  constexpr const char* HourglassTalentConfig_get_Cost = "F2 0F 10 41 48 C3 CC CC";

  // 0xB2FBA0
  constexpr const char* HourglassUpgradeItem_GetCost =
    "48 89 5C 24 08 57 48 83 EC 50 80 3D ? ? ? ? 00 8B FA 0F 29 74 24 40";

  // 0x9AF3D0
  constexpr const char* IAPRewarder_Reward =
    "48 89 5C 24 10 48 89 6C 24 18 56 57 41 57 48 83 EC 20 80 3D ? ? ? ? 00 45 0F B6 F9 41 0F B6 F0";

  // 0x677980
  constexpr const char* ItemDetail_get_Amount = "48 8B 41 18 C3 CC CC CC CC CC CC CC CC CC CC CC F3 0F 11 44 24 08";

  // 0x720320
  constexpr const char* MapUi_UpdatePlayerLocation =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 8B 10 01 00 00";

  // 0x7EBF30
  constexpr const char* MarketLotScriptableObject_CurrentPrice =
    "48 89 5C 24 10 48 89 74 24 18 57 48 83 EC 20 80 3D ? ? ? ? 00 8B DA 48 8B F9 75 7F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 "
    "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? E8 ? ? ? ? 33 D2 48 8B C8 48 8B F0 E8 ? ? ? ? 48 85 F6 "
    "0F 84 ? ? ? ? 48 8D 4E 18";

  // 0x7EC320
  constexpr const char* MarketLot_GetCurrentPrice =
    "48 89 74 24 18 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B F9 0F 29 74 24 20 0F 28 F1 75 7F 48 8D 0D ? ? ? ?";

  // 0x6EB750
  constexpr const char* MetaModel_get_IsCheater = "80 79 38 00 75 17 80 79 39 00";

  // 0xBF4590
  constexpr const char* MobKillTrigger_Apply = "48 89 5C 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 13 "
                                               "48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 63 43 20";

  // 0xBF4750
  constexpr const char* MobKillTrigger_CurrentKills =
    "40 53 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 4B 28 48 89 "
    "7C 24 30 48 85 C9 74 42 48 8B 05 ? ? ? ?";

  // 0xBF4810
  constexpr const char* MobKillTrigger_OnDisable =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F1 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 88 A0 00 00 00 48 8B 79 10 48 8B 0D ? ? ? ? E8 ? ? ? ? "
    "4C 8B 05 ? ? ? ? 45 33 C9 48 8B D6 48 8B C8 48 8B D8 E8 ? ? ? ? 45 33 C0 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B D8 48 "
    "85 C0 75 16 48 8B 05 ? ? ? ? 33 D2 48 8B 88 A0 00 00 00 48 89 59 10 EB 47 48 8B 3D ? ? ? ? 48 8B CB 48 8B D7 E8 ? "
    "? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 91 A0 00 00 00 48 8B CB 48 89 42 10 48 8B 3D ? ? ? ? 48 8B D7 "
    "E8 ? ? ? ? 48 8B D0 48 85 C0 74 79 48 8B 05 ? ? ? ?";

  // 0xBF49B0
  constexpr const char* MobKillTrigger_OnEnable =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F1 75 4F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D "
    "? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? "
    "? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 48 8B 88 A0 00 00 00 48 8B 79 10 48 8B 0D ? ? ? ? E8 ? ? ? ? "
    "4C 8B 05 ? ? ? ? 45 33 C9 48 8B D6 48 8B C8 48 8B D8 E8 ? ? ? ? 45 33 C0 48 8B D3 48 8B CF E8 ? ? ? ? 48 8B D8 48 "
    "85 C0 75 16 48 8B 05 ? ? ? ? 33 D2 48 8B 88 A0 00 00 00 48 89 59 10 EB 47 48 8B 3D ? ? ? ? 48 8B CB 48 8B D7 E8 ? "
    "? ? ? 48 85 C0 0F 84 ? ? ? ? 48 8B 0D ? ? ? ? 48 8B 91 A0 00 00 00 48 8B CB 48 89 42 10 48 8B 3D ? ? ? ? 48 8B D7 "
    "E8 ? ? ? ? 48 8B D0 48 85 C0 74 76 48 8B 05 ? ? ? ?";

  // 0xBF47D0
  constexpr const char* MobKillTrigger_Start = "40 53 48 83 EC 20 83 79 20 00 48 8B D9 7E 21 48 83 79 28 00";

  // 0x8E75F0
  constexpr const char* MobilePurchaseService_InitiatePurchase =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? "
    "01 80 7B 4C 00 48 C7 44 24 30 ? ? ? ? 75 78";

  // 0x70D0B0
  constexpr const char* MovementControl_Move =
    "40 53 48 81 EC ? ? ? ? 80 3D ? ? ? ? 00 48 8B D9 0F 29 BC 24 E0 00 00 00";

  // 0x6A3290
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 49 8B E8 48 8B DA 4C 8B F1";

  // 0x6A3360
  constexpr const char* ObscuredCheatingDetector_OnCheatingDetected_1 =
    "40 53 55 56 41 56 48 83 EC 28 80 3D ? ? ? ? 00 49 8B F1 41 0F B6 E8";

  // 0x68BDC0
  constexpr const char* ObscuredFilePrefs_OnDataFromAnotherDeviceDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 40 08";

  // 0x68BE30
  constexpr const char* ObscuredFilePrefs_OnNotGenuineDataDetected =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 4C 8B 00";

  // 0x8A28F0
  constexpr const char* OfflineProgressService_CanUseTimeskip =
    "48 89 5C 24 10 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B D9 75 1F 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? C6 05 ? ? ? ? 01 48 8B 0D ? ? ? ? 48 C7 44 24 30 ? ? ? ?";

  // 0x8A35A0
  constexpr const char* OfflineProgressService_CreateActivitySnapshot =
    "48 89 5C 24 10 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B F9 75 4F 48 8D 0D ? ? ? ?";

  // 0x8A3940
  constexpr const char* OfflineProgressService_CurrentActivitySnapshot =
    "48 89 5C 24 08 57 48 83 EC 20 48 8B F9 48 8B 49 58 48 85 C9";

  // 0x66C5A0
  constexpr const char* OfflineProgressService_GetCombatCalculator =
    "48 8B 41 50 C3 CC CC CC CC CC CC CC CC CC CC CC 0F B6 41 58";

  // 0x8A5300
  constexpr const char* OfflineProgressService_SetCurrentActivity =
    "48 89 5C 24 08 57 48 83 EC 40 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 5B 48 8D 0D ? ? ? ?";

  // 0x8A6A80
  constexpr const char* OfflineProgressService_WriteClearedActivity =
    "48 89 5C 24 08 57 48 83 EC 20 48 8B 41 28 48 8B D9 48 85 C0 0F 84 ? ? ? ?";

  // 0x9181A0
  constexpr const char* PlayerCharacter_TakeDamage = "48 83 EC 38 80 3D ? ? ? ? 00 0F 29 74 24 20 0F 28 F1 75 13 48 8D "
                                                     "0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ?";

  // 0x9183E0
  constexpr const char* PlayerCharacter_ctor =
    "48 89 5C 24 08 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B F9 75 37 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? "
    "? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 C7 87 B8 00 00 00 ? ? ? ?";

  // 0x8528F0
  constexpr const char* PositionerService_TryChangeScene =
    "48 89 5C 24 08 48 89 74 24 10 57 48 83 EC 30 80 3D ? ? ? ? 00 41 0F B6 F8 48 8B F2 48 8B D9 75 1F";

  // 0xAC7D80
  constexpr const char* RealmAttackReceiver_Recieve =
    "48 89 74 24 20 41 56 48 83 EC 60 80 3D ? ? ? ? 00 4C 8B F2 48 8B F1 75 73 48 8D 0D ? ? ? ?";

  // 0xBE48A0
  constexpr const char* SacrificeUI_Buy =
    "40 56 57 48 83 EC 48 80 3D ? ? ? ? 00 48 8B F2 48 8B F9 75 73 48 8D 0D ? ? ? ?";

  // 0xBE5080
  constexpr const char* SacrificeUI_Select =
    "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B E9";

  // 0xBE5870
  constexpr const char* SacrificeUI_TryRollSacrificeHalfCost =
    "48 83 EC 38 33 D2 0F 29 74 24 20 B9 ? ? ? ? E8 ? ? ? ? 33 C9 0F 28 F0 E8 ? ? ? ? 0F 2F F0 0F 28 74 24 20 0F 97 C0 "
    "48 83 C4 38 C3 CC 40 53 48 83 EC 20";

  // 0x7F0C90
  constexpr const char* ShopSpendingService_CanSpend =
    "48 89 5C 24 10 48 89 74 24 18 57 48 81 EC ? ? ? ? 0F 29 74 24 70 41 8B F0 48 8B FA";

  // 0x7F0F90
  constexpr const char* ShopSpendingService_Spend = "48 8B C4 48 89 58 18 48 89 50 10 48 89 48 08 56 57 41 54 41 56 41 "
                                                    "57 48 81 EC ? ? ? ? 0F 29 70 C8 0F 29 78 B8 41 8B D8 4C 8B FA";

  // 0x7F23A0
  constexpr const char* Shop_TryBuy = "48 89 5C 24 18 48 89 6C 24 20 56 48 83 EC 40 80 3D ? ? ? ? 00 48 8B DA";

  // 0x7F2AD0
  constexpr const char* Shop_get_Discount = "40 53 48 83 EC 20 80 79 60 00 48 8B D9 74 2A 48 8B 49 48";

  // 0x9193C0
  constexpr const char* Skill_AddExperience = "40 53 48 83 EC 70 0F 10 41 30";

  // 0x6BC650
  constexpr const char* SpeedHackDetector_IsTicksCheated =
    "40 53 55 56 57 48 83 EC 38 80 3D ? ? ? ? 00 49 8B F1 0F 29 74 24 20";

  // 0x6BC6F0
  constexpr const char* SpeedHackDetector_IsTimeScaleCheated = "40 53 48 83 EC 30 80 79 7D 00";

  // 0x85AF10
  constexpr const char* StatisticsService_AddKillWithPortalProgress =
    "40 53 55 56 57 48 83 EC 38 49 8B F9 48 C7 44 24 20 ? ? ? ?";

  // 0x85B280
  constexpr const char* StatisticsService_AddPortalKill = "40 55 57 41 54 41 56 41 57 48 83 EC 40 80 3D ? ? ? ? 00";

  // 0x8EACC0
  constexpr const char* SteamPurchaseService_FindLot =
    "48 83 EC 28 48 8B 49 28 48 85 C9 74 0C 45 33 C0 48 83 C4 28 E9 ? ? ? ? E8 ? ? ? ? CC CC 48 89 5C 24 08";

  // 0x8EAF00
  constexpr const char* SteamPurchaseService_InitiatePurchase =
    "48 89 5C 24 20 57 48 83 EC 20 80 3D ? ? ? ? 00 48 8B FA 48 8B D9 75 73 48 8D 0D ? ? ? ?";

  // 0x6BF190
  constexpr const char* TimeCheatingDetector_ReportCheckResult =
    "41 56 48 83 EC 20 80 3D ? ? ? ? 00 4C 8B F1 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 49 8B 86 D8 00 00 "
    "00";

  // 0x3C3CB90
  constexpr const char* Time_set_timeScale =
    "48 83 EC 38 48 8B 05 ? ? ? ? 0F 29 74 24 20 0F 28 F0 48 85 C0 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 05 ? ? ? ? "
    "0F 28 C6 0F 28 74 24 20 48 83 C4 38 48 FF E0 CC CC CC CC CC CC 48 89 5C 24 08 57 48 83 EC 20 48 8B FA 48 8B D9";

  // 0x9C0190
  constexpr const char* TimeskipItem_get_CanUseImpl =
    "48 83 EC 28 80 3D ? ? ? ? 00 75 13 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 48 8B 05 ? ? ? ? 83 B8 E4 00 00 "
    "00 00 75 0F 48 8B C8 E8 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 80 A0 00 00 00 48 8B 08 48 85 C9 74 0B 33 D2 48 83 C4 28 "
    "E9 ? ? ? ? E8 ? ? ? ? CC CC 40 57";

  // 0x7F8780
  constexpr const char* Trader_CanSpend_1 = "48 89 5C 24 08 57 48 83 EC 30 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 13 48 "
                                            "8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 4C 8B 03 4D 85 C0";

  // 0x7F8BC0
  constexpr const char* Trader_Spend_1 =
    "48 89 5C 24 08 57 48 83 EC 70 80 3D ? ? ? ? 00 48 8B DA 48 8B F9 75 2B 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? "
    "? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? C6 05 ? ? ? ? 01 4C 8B 03 0F 57 C0";

  // 0xBFD870
  constexpr const char* WorldElite_Update =
    "40 53 48 83 EC 50 0F 29 74 24 40 33 D2 0F 29 7C 24 30 48 8B D9 E8 ? ? ? ? 48 85 C0 0F 84 ? ? ? ? 45 33 C0 48 8D "
    "4C 24 20 48 8B D0 E8 ? ? ? ? F3 0F 10 35 ? ? ? ?";

  // 0xC00D40
  constexpr const char* YellowDragonBoss_Update = "40 53 48 83 EC 50 80 79 3C 00";

}  // namespace Signatures
