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

#include "HourglassUpgrades.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

#include <cstdint>
#include <unordered_map>

namespace Features
{
  namespace HourglassUpgrades
  {
    thread_local bool bIsPurchasing  = false;
    thread_local bool bInBuyTimeEcho = false;

    static std::unordered_map<void*, double> s_OriginalTalentCosts;

    bool IsPurchasing() { return bIsPurchasing; }

    // =========================================================================
    // Level 1: Free Sand Upgrade (Sand Generators & Hourglass Level-Up Cost)
    // =========================================================================

    double (*Orig_HourglassService_CostFactor)(void* __this, void* genSpec, void* method_info);
    double Hook_HourglassService_CostFactor(void* __this, void* genSpec, void* method_info)
    {
      if (Menu::Config.bFreeSandUpgrade)
        return 0.0;
      return Orig_HourglassService_CostFactor(__this, genSpec, method_info);
    }

    double (*Orig_HourglassService_BulkCost)(void* __this, void* id, int64_t n, void* method_info);
    double Hook_HourglassService_BulkCost(void* __this, void* id, int64_t n, void* method_info)
    {
      if (Menu::Config.bFreeSandUpgrade)
        return 0.0;
      return Orig_HourglassService_BulkCost(__this, id, n, method_info);
    }

    int64_t (*Orig_HourglassService_MaxAffordable)(void* __this, void* id, void* method_info);
    int64_t Hook_HourglassService_MaxAffordable(void* __this, void* id, void* method_info)
    {
      int64_t count = Orig_HourglassService_MaxAffordable(__this, id, method_info);
      if (Menu::Config.bFreeSandUpgrade && count <= 0)
        return 1;
      return count;
    }

    bool (*Orig_HourglassService_BuyGenerator)(void* __this, void* id, int64_t amount, void* method_info);
    bool Hook_HourglassService_BuyGenerator(void* __this, void* id, int64_t amount, void* method_info)
    {
      if (Menu::Config.bFreeSandUpgrade && amount <= 0)
        amount = 1;
      return Orig_HourglassService_BuyGenerator(__this, id, amount, method_info);
    }

    double (*Orig_HourglassService_LevelCost)(void* __this, void* method_info);
    double Hook_HourglassService_LevelCost(void* __this, void* method_info)
    {
      if (Menu::Config.bFreeSandUpgrade)
        return 0.0;
      return Orig_HourglassService_LevelCost(__this, method_info);
    }

    double (*Orig_HourglassLevelConfig_SandCost)(void* __this, int targetLevel, void* method_info);
    double Hook_HourglassLevelConfig_SandCost(void* __this, int targetLevel, void* method_info)
    {
      if (Menu::Config.bFreeSandUpgrade)
        return 0.0;
      return Orig_HourglassLevelConfig_SandCost(__this, targetLevel, method_info);
    }

    // =========================================================================
    // Level 2: Free Upgrade Generator (Generator Upgrades & Upgrade Blocks)
    // =========================================================================

    double (*Orig_HourglassService_UpgradeCost)(void* __this, void* id, void* method_info);
    double Hook_HourglassService_UpgradeCost(void* __this, void* id, void* method_info)
    {
      if (Menu::Config.bFreeUpgradeGenerator)
        return 0.0;
      return Orig_HourglassService_UpgradeCost(__this, id, method_info);
    }

    double (*Orig_HourglassUpgradeItem_GetCost)(void* __this, int currentLevel, void* method_info);
    double Hook_HourglassUpgradeItem_GetCost(void* __this, int currentLevel, void* method_info)
    {
      if (Menu::Config.bFreeUpgradeGenerator)
        return 0.0;
      return Orig_HourglassUpgradeItem_GetCost(__this, currentLevel, method_info);
    }

    bool (*Orig_HourglassService_BuyUpgrade)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyUpgrade(void* __this, void* id, void* method_info)
    {
      return Orig_HourglassService_BuyUpgrade(__this, id, method_info);
    }

    void (*Orig_HourglassService_BuyUpgradeBlock)(void* __this, void* block, void* method_info);
    void Hook_HourglassService_BuyUpgradeBlock(void* __this, void* block, void* method_info)
    {
      Orig_HourglassService_BuyUpgradeBlock(__this, block, method_info);
    }

    // =========================================================================
    // Level 4: Free Awakening Generator (Hard Upgrades)
    // =========================================================================

    int64_t (*Orig_HourglassHardUUpgrade_GetCostAmount)(void* __this, int nextLevel, void* method_info);
    int64_t Hook_HourglassHardUUpgrade_GetCostAmount(void* __this, int nextLevel, void* method_info)
    {
      if (Menu::Config.bFreeAwakeningGenerator)
        return 0;
      return Orig_HourglassHardUUpgrade_GetCostAmount(__this, nextLevel, method_info);
    }

    bool (*Orig_HourglassService_CanAffordHardUpgrade)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_CanAffordHardUpgrade(void* __this, void* id, void* method_info)
    {
      bool prev     = bIsPurchasing;
      bIsPurchasing = prev || Menu::Config.bFreeAwakeningGenerator;
      bool res      = Orig_HourglassService_CanAffordHardUpgrade(__this, id, method_info);
      bIsPurchasing = prev;
      return res;
    }

    bool (*Orig_HourglassService_BuyHardUpgrade)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyHardUpgrade(void* __this, void* id, void* method_info)
    {
      bool prev     = bIsPurchasing;
      bIsPurchasing = prev || Menu::Config.bFreeAwakeningGenerator;
      bool res      = Orig_HourglassService_BuyHardUpgrade(__this, id, method_info);
      bIsPurchasing = prev;
      return res;
    }

    bool (*Orig_Trader_CanSpend_1)(void* __this, void* price, void* method_info);
    bool Hook_Trader_CanSpend_1(void* __this, void* price, void* method_info)
    {
      if (bIsPurchasing)
        return true;
      return Orig_Trader_CanSpend_1(__this, price, method_info);
    }

    bool (*Orig_Trader_Spend_1)(void* __this, void* price, void* method_info);
    bool Hook_Trader_Spend_1(void* __this, void* price, void* method_info)
    {
      if (bIsPurchasing)
        return true;
      return Orig_Trader_Spend_1(__this, price, method_info);
    }

    // =========================================================================
    // Timeline Upgrade Tree & Restoration of Eternity (Talent Trees & Echoes)
    // =========================================================================

    void* (*Orig_HourglassService_FindTalent)(void* __this, void* talentId, void* method_info);
    void* (*Orig_HourglassService_TreeOf)(void* __this, void* talent, void* method_info);

    static void ApplyTalentTreeCost(void* talent, void* tree)
    {
      if (!talent || !tree)
        return;

      // HourglassTreeCurrency: 0 = Prestige (Timeline tree), 1 = Essence (Restoration tree)
      int32_t currency = *(int32_t*) ((uintptr_t) tree + Offsets::currency);
      bool    isFree =
        (currency == 0 && Menu::Config.bFreeTimelineNodes) || (currency == 1 && Menu::Config.bFreeRestorationNodes);

      double* pCost = (double*) ((uintptr_t) talent + Offsets::cost);
      if (isFree) {
        if (*pCost != 0.0) {
          s_OriginalTalentCosts[talent] = *pCost;
          *pCost                        = 0.0;
        }
      }
      else {
        if (*pCost == 0.0) {
          auto it = s_OriginalTalentCosts.find(talent);
          if (it != s_OriginalTalentCosts.end()) {
            *pCost = it->second;
          }
        }
      }
    }

    void* Hook_HourglassService_TreeOf(void* __this, void* talent, void* method_info)
    {
      void* tree = Orig_HourglassService_TreeOf(__this, talent, method_info);
      ApplyTalentTreeCost(talent, tree);
      return tree;
    }

    bool (*Orig_HourglassService_CanBuyTalent)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_CanBuyTalent(void* __this, void* id, void* method_info)
    {
      if (__this && id && Orig_HourglassService_FindTalent && Orig_HourglassService_TreeOf) {
        void* talent = Orig_HourglassService_FindTalent(__this, id, nullptr);
        if (talent) {
          void* tree = Orig_HourglassService_TreeOf(__this, talent, nullptr);
          ApplyTalentTreeCost(talent, tree);
        }
      }
      return Orig_HourglassService_CanBuyTalent(__this, id, method_info);
    }

    bool (*Orig_HourglassService_BuyTalent)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyTalent(void* __this, void* id, void* method_info)
    {
      if (__this && id && Orig_HourglassService_FindTalent && Orig_HourglassService_TreeOf) {
        void* talent = Orig_HourglassService_FindTalent(__this, id, nullptr);
        if (talent) {
          void* tree = Orig_HourglassService_TreeOf(__this, talent, nullptr);
          ApplyTalentTreeCost(talent, tree);
        }
      }
      return Orig_HourglassService_BuyTalent(__this, id, method_info);
    }

    double (*Orig_HourglassService_TimeEchoBatchCost)(void* __this, int64_t count, void* method_info);
    double Hook_HourglassService_TimeEchoBatchCost(void* __this, int64_t count, void* method_info)
    {
      if (Menu::Config.bFreeRestorationNodes)
        return 0.0;
      return Orig_HourglassService_TimeEchoBatchCost(__this, count, method_info);
    }

    int64_t (*Orig_HourglassCalc_GeometricMaxAffordable)(
      double baseCost, double growth, int64_t owned, double balance, void* method_info
    );
    int64_t Hook_HourglassCalc_GeometricMaxAffordable(
      double baseCost, double growth, int64_t owned, double balance, void* method_info
    )
    {
      int64_t count = Orig_HourglassCalc_GeometricMaxAffordable(baseCost, growth, owned, balance, method_info);
      if (bInBuyTimeEcho && Menu::Config.bFreeRestorationNodes && count <= 0)
        return 1;
      return count;
    }

    bool (*Orig_HourglassService_BuyTimeEcho)(void* __this, void* method_info);
    bool Hook_HourglassService_BuyTimeEcho(void* __this, void* method_info)
    {
      bInBuyTimeEcho = true;
      bool res       = Orig_HourglassService_BuyTimeEcho(__this, method_info);
      bInBuyTimeEcho = false;
      return res;
    }

    void Initialize()
    {
      // Level 1: Free Sand Upgrade
      HOOK_SIGNATURE(
        "HourglassService::CostFactor", Signatures::HourglassService_CostFactor, Hook_HourglassService_CostFactor,
        Orig_HourglassService_CostFactor
      );
      HOOK_SIGNATURE(
        "HourglassService::BulkCost", Signatures::HourglassService_BulkCost, Hook_HourglassService_BulkCost,
        Orig_HourglassService_BulkCost
      );
      HOOK_SIGNATURE(
        "HourglassService::MaxAffordable", Signatures::HourglassService_MaxAffordable,
        Hook_HourglassService_MaxAffordable, Orig_HourglassService_MaxAffordable
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyGenerator", Signatures::HourglassService_BuyGenerator, Hook_HourglassService_BuyGenerator,
        Orig_HourglassService_BuyGenerator
      );
      HOOK_SIGNATURE(
        "HourglassService::LevelCost", Signatures::HourglassService_LevelCost, Hook_HourglassService_LevelCost,
        Orig_HourglassService_LevelCost
      );
      HOOK_SIGNATURE(
        "HourglassLevelConfig::SandCost", Signatures::HourglassLevelConfig_SandCost, Hook_HourglassLevelConfig_SandCost,
        Orig_HourglassLevelConfig_SandCost
      );

      // Level 2: Free Upgrade Generator
      HOOK_SIGNATURE(
        "HourglassService::UpgradeCost", Signatures::HourglassService_UpgradeCost, Hook_HourglassService_UpgradeCost,
        Orig_HourglassService_UpgradeCost
      );
      HOOK_SIGNATURE(
        "HourglassUpgradeItem::GetCost", Signatures::HourglassUpgradeItem_GetCost, Hook_HourglassUpgradeItem_GetCost,
        Orig_HourglassUpgradeItem_GetCost
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyUpgrade", Signatures::HourglassService_BuyUpgrade, Hook_HourglassService_BuyUpgrade,
        Orig_HourglassService_BuyUpgrade
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyUpgradeBlock", Signatures::HourglassService_BuyUpgradeBlock,
        Hook_HourglassService_BuyUpgradeBlock, Orig_HourglassService_BuyUpgradeBlock
      );

      // Level 4: Free Awakening Generator
      HOOK_SIGNATURE(
        "HourglassHardUUpgrade::GetCostAmount", Signatures::HourglassHardUUpgrade_GetCostAmount,
        Hook_HourglassHardUUpgrade_GetCostAmount, Orig_HourglassHardUUpgrade_GetCostAmount
      );
      HOOK_SIGNATURE(
        "HourglassService::CanAffordHardUpgrade", Signatures::HourglassService_CanAffordHardUpgrade,
        Hook_HourglassService_CanAffordHardUpgrade, Orig_HourglassService_CanAffordHardUpgrade
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyHardUpgrade", Signatures::HourglassService_BuyHardUpgrade,
        Hook_HourglassService_BuyHardUpgrade, Orig_HourglassService_BuyHardUpgrade
      );
      HOOK_SIGNATURE("Trader::CanSpend", Signatures::Trader_CanSpend_1, Hook_Trader_CanSpend_1, Orig_Trader_CanSpend_1);
      HOOK_SIGNATURE("Trader::Spend", Signatures::Trader_Spend_1, Hook_Trader_Spend_1, Orig_Trader_Spend_1);

      // Timeline Upgrade Tree & Restoration of Eternity
      HOOK_SIGNATURE(
        "HourglassService::FindTalent", Signatures::HourglassService_FindTalent, nullptr,
        Orig_HourglassService_FindTalent
      );
      HOOK_SIGNATURE(
        "HourglassService::TreeOf", Signatures::HourglassService_TreeOf, Hook_HourglassService_TreeOf,
        Orig_HourglassService_TreeOf
      );
      HOOK_SIGNATURE(
        "HourglassService::CanBuyTalent", Signatures::HourglassService_CanBuyTalent, Hook_HourglassService_CanBuyTalent,
        Orig_HourglassService_CanBuyTalent
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyTalent", Signatures::HourglassService_BuyTalent, Hook_HourglassService_BuyTalent,
        Orig_HourglassService_BuyTalent
      );
      HOOK_SIGNATURE(
        "HourglassService::TimeEchoBatchCost", Signatures::HourglassService_TimeEchoBatchCost,
        Hook_HourglassService_TimeEchoBatchCost, Orig_HourglassService_TimeEchoBatchCost
      );
      HOOK_SIGNATURE(
        "HourglassCalc::GeometricMaxAffordable", Signatures::HourglassCalc_GeometricMaxAffordable,
        Hook_HourglassCalc_GeometricMaxAffordable, Orig_HourglassCalc_GeometricMaxAffordable
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyTimeEcho", Signatures::HourglassService_BuyTimeEcho, Hook_HourglassService_BuyTimeEcho,
        Orig_HourglassService_BuyTimeEcho
      );
    }

    void Uninitialize() { }
  }  // namespace HourglassUpgrades
}  // namespace Features
