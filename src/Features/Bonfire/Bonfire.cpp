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

#include "Bonfire.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include "../../Modules/Hooks/Utils.hpp"

namespace Features
{
  namespace Bonfire
  {
    bool (*Orig_BonfireService_get_Lit)(void* __this, void* method_info);
    bool Hook_BonfireService_get_Lit(void* __this, void* method_info)
    {
      if (Menu::Config.bAlwaysLitBonfire) {
        return true;
      }
      return Orig_BonfireService_get_Lit(__this, method_info);
    }

    float (*Orig_BonfireFuelBurner_get_Fuel)(void* __this, void* method_info);
    float (*Orig_BonfireFuelBurner_get_MaxFuel)(void* __this, void* method_info);
    float Hook_BonfireFuelBurner_get_Fuel(void* __this, void* method_info)
    {
      if (Menu::Config.bAlwaysLitBonfire) {
        if (Orig_BonfireFuelBurner_get_MaxFuel) {
          return Orig_BonfireFuelBurner_get_MaxFuel(__this, nullptr);
        }
        return 150000.0f;
      }
      return Orig_BonfireFuelBurner_get_Fuel(__this, method_info);
    }

    enum class BonfireContext
    {
      None,
      AshUpgrade,
      Sacrifice
    };

    thread_local BonfireContext currentContext = BonfireContext::None;
    thread_local bool           bIsBuying      = false;

    void (*Orig_EnhancementTreeUI_Select)(void* __this, void* marketLot, void* method_info);
    void Hook_EnhancementTreeUI_Select(void* __this, void* marketLot, void* method_info)
    {
      currentContext = BonfireContext::AshUpgrade;
      Orig_EnhancementTreeUI_Select(__this, marketLot, method_info);
      currentContext = BonfireContext::None;
    }

    void (*Orig_EnhancementTreeUI_Buy)(void* __this, void* lot, void* method_info);
    void Hook_EnhancementTreeUI_Buy(void* __this, void* lot, void* method_info)
    {
      currentContext = BonfireContext::AshUpgrade;
      if (Menu::Config.bFreeAshUpgrade) {
        bIsBuying = true;
      }
      Orig_EnhancementTreeUI_Buy(__this, lot, method_info);
      bIsBuying      = false;
      currentContext = BonfireContext::None;
    }

    void (*Orig_SacrificeUI_Select)(void* __this, void* lot, void* method_info);
    void Hook_SacrificeUI_Select(void* __this, void* lot, void* method_info)
    {
      currentContext = BonfireContext::Sacrifice;
      Orig_SacrificeUI_Select(__this, lot, method_info);
      currentContext = BonfireContext::None;
    }

    void (*Orig_SacrificeUI_Buy)(void* __this, void* lot, void* method_info);
    void Hook_SacrificeUI_Buy(void* __this, void* lot, void* method_info)
    {
      currentContext = BonfireContext::Sacrifice;
      if (Menu::Config.bFreeSacrificeCost) {
        bIsBuying = true;
      }
      Orig_SacrificeUI_Buy(__this, lot, method_info);
      bIsBuying      = false;
      currentContext = BonfireContext::None;
    }

    void* (*Orig_MarketLotScriptableObject_CurrentPrice)(void* __this, int tier, void* method_info);
    void* Hook_MarketLotScriptableObject_CurrentPrice(void* __this, int tier, void* method_info)
    {
      void* dict = Orig_MarketLotScriptableObject_CurrentPrice(__this, tier, method_info);
      if (!dict) {
        return dict;
      }

      if (currentContext == BonfireContext::AshUpgrade) {
        if (Menu::Config.bFreeAshUpgrade) {
          if (bIsBuying) {
            Utils::Il2Cpp::ClearDictionary(dict);
          }
          else {
            Utils::Il2Cpp::ZeroDictionaryFloatValues(dict);
          }
        }
      }
      else if (currentContext == BonfireContext::Sacrifice) {
        if (Menu::Config.bFreeSacrificeCost) {
          if (bIsBuying) {
            Utils::Il2Cpp::ClearDictionary(dict);
          }
          else {
            Utils::Il2Cpp::ZeroDictionaryFloatValues(dict);
          }
        }
      }
      return dict;
    }


    bool IsFreeSpending()
    {
      return (currentContext == BonfireContext::Sacrifice && Menu::Config.bFreeSacrificeCost)
          || (currentContext == BonfireContext::AshUpgrade && Menu::Config.bFreeAshUpgrade);
    }

    bool (*Orig_SacrificeUI_TryRollSacrificeHalfCost)(void* method_info);
    bool Hook_SacrificeUI_TryRollSacrificeHalfCost(void* method_info)
    {
      if (Menu::Config.bFreeSacrificeCost) {
        return true;  // Always succeed half cost roll just in case
      }
      return Orig_SacrificeUI_TryRollSacrificeHalfCost(method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "BonfireService::get_Lit", Signatures::BonfireService_get_Lit, Hook_BonfireService_get_Lit,
        Orig_BonfireService_get_Lit
      );
      HOOK_SIGNATURE(
        "BonfireFuelBurner::get_Fuel", Signatures::BonfireFuelBurner_get_Fuel, Hook_BonfireFuelBurner_get_Fuel,
        Orig_BonfireFuelBurner_get_Fuel
      );
      HOOK_SIGNATURE(
        "BonfireFuelBurner::get_MaxFuel", Signatures::BonfireFuelBurner_get_MaxFuel, nullptr,
        Orig_BonfireFuelBurner_get_MaxFuel
      );
      HOOK_SIGNATURE(
        "EnhancementTreeUI::Buy", Signatures::EnhancementTreeUI_Buy, Hook_EnhancementTreeUI_Buy,
        Orig_EnhancementTreeUI_Buy
      );
      HOOK_SIGNATURE(
        "EnhancementTreeUI::Select", Signatures::EnhancementTreeUI_Select, Hook_EnhancementTreeUI_Select,
        Orig_EnhancementTreeUI_Select
      );

      HOOK_SIGNATURE(
        "MarketLotScriptableObject::CurrentPrice", Signatures::MarketLotScriptableObject_CurrentPrice,
        Hook_MarketLotScriptableObject_CurrentPrice, Orig_MarketLotScriptableObject_CurrentPrice
      );

      HOOK_SIGNATURE("SacrificeUI::Buy", Signatures::SacrificeUI_Buy, Hook_SacrificeUI_Buy, Orig_SacrificeUI_Buy);
      HOOK_SIGNATURE(
        "SacrificeUI::Select", Signatures::SacrificeUI_Select, Hook_SacrificeUI_Select, Orig_SacrificeUI_Select
      );
      HOOK_SIGNATURE(
        "SacrificeUI::TryRollSacrificeHalfCost", Signatures::SacrificeUI_TryRollSacrificeHalfCost,
        Hook_SacrificeUI_TryRollSacrificeHalfCost, Orig_SacrificeUI_TryRollSacrificeHalfCost
      );
    }

    void Uninitialize() { }
  }  // namespace Bonfire
}  // namespace Features
