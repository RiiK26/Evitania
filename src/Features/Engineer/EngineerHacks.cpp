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

#include "EngineerHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Hooks/Utils.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace EngineerHacks
  {
    thread_local bool bIsBuyingUpgrade = false;

    bool (*Orig_EngineerService_TryGetUpgradeCost)(
      void* __this, int slotIndex, void* upgradeId, void* out_cost, void* method_info
    );
    bool Hook_EngineerService_TryGetUpgradeCost(
      void* __this, int slotIndex, void* upgradeId, void* out_cost, void* method_info
    )
    {
      bool result = Orig_EngineerService_TryGetUpgradeCost(__this, slotIndex, upgradeId, out_cost, method_info);
      if (Menu::Config.bFreeEngineerUpgrades && result && out_cost) {
        // UpgradeCost struct:
        // 0x0: TradableBase Item
        // 0x8: long Amount
        *(long long*) ((uintptr_t) out_cost + Offsets::Amount) = 0;
      }
      return result;
    }

    bool (*Orig_EngineerSystemUI_TryBuyUpgrade)(void* __this, void* config, void* method_info);
    bool Hook_EngineerSystemUI_TryBuyUpgrade(void* __this, void* config, void* method_info)
    {
      if (Menu::Config.bFreeEngineerUpgrades) {
        bIsBuyingUpgrade = true;
      }
      bool result      = Orig_EngineerSystemUI_TryBuyUpgrade(__this, config, method_info);
      bIsBuyingUpgrade = false;
      return result;
    }

    void* (*Orig_EngineerUpgradeConfig_GetPrice)(void* __this, int nextTier, void* method_info);
    void* Hook_EngineerUpgradeConfig_GetPrice(void* __this, int nextTier, void* method_info)
    {
      void* dict = Orig_EngineerUpgradeConfig_GetPrice(__this, nextTier, method_info);
      if (Menu::Config.bFreeEngineerUpgrades && dict) {
        if (bIsBuyingUpgrade) {
          // In TryBuyUpgrade, clear the dictionary so Trader.Spend succeeds without stockpile failure
          Utils::Il2Cpp::ClearDictionary(dict);
        }
        else {
          // In UI display, preserve the currency keys (for icons) and zero the price values
          Utils::Il2Cpp::ZeroDictionaryFloatValues(dict);
        }
      }
      return dict;
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "EngineerService::TryGetUpgradeCost", Signatures::EngineerService_TryGetUpgradeCost,
        Hook_EngineerService_TryGetUpgradeCost, Orig_EngineerService_TryGetUpgradeCost
      );

      HOOK_SIGNATURE(
        "EngineerSystemUI::TryBuyUpgrade", Signatures::EngineerSystemUI_TryBuyUpgrade,
        Hook_EngineerSystemUI_TryBuyUpgrade, Orig_EngineerSystemUI_TryBuyUpgrade
      );

      HOOK_SIGNATURE(
        "EngineerUpgradeConfig::GetPrice", Signatures::EngineerUpgradeConfig_GetPrice,
        Hook_EngineerUpgradeConfig_GetPrice, Orig_EngineerUpgradeConfig_GetPrice
      );
    }

    void Uninitialize() { }
  }  // namespace EngineerHacks
}  // namespace Features
