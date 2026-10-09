#include "Bonfire.hpp"
#include "../../Modules/Hooks/Hooks.hpp"       // IWYU pragma: keep
#include "../../Modules/Hooks/Signatures.hpp"  // IWYU pragma: keep
#include "../../Modules/Menu/Menu.hpp"         // IWYU pragma: keep
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

    void (*Orig_SacrificeUI_Buy)(void* __this, void* lot, void* method_info);
    bool (*Orig_ShopSpendingService_Spend)(void* __this, void* price, void* source, void* method_info);
    void (*Orig_EnhancementTreeUI_Buy)(void* __this, void* lot, void* method_info);

    void Hook_EnhancementTreeUI_Buy(void* __this, void* lot, void* method_info)
    {
      Orig_EnhancementTreeUI_Buy(__this, lot, method_info);
    }

    void Hook_SacrificeUI_Buy(void* __this, void* lot, void* method_info)
    {
      Orig_SacrificeUI_Buy(__this, lot, method_info);
    }

    void* (*Orig_MarketLotScriptableObject_CurrentPrice)(void* __this, int tier, void* method_info);
    void* Hook_MarketLotScriptableObject_CurrentPrice(void* __this, int tier, void* method_info)
    {
      void* dict = Orig_MarketLotScriptableObject_CurrentPrice(__this, tier, method_info);
      if (Menu::Config.bFreeAshUpgrade || Menu::Config.bFreeSacrificeCost || Menu::Config.bFreeStore) {
        if (dict) {
          Utils::Il2Cpp::ClearDictionary(dict);
        }
      }
      return dict;
    }

    void* (*Orig_MarketLot_GetCurrentPrice)(void* __this, float discount, void* method_info);
    void* Hook_MarketLot_GetCurrentPrice(void* __this, float discount, void* method_info)
    {
      void* dict = Orig_MarketLot_GetCurrentPrice(__this, discount, method_info);
      if (Menu::Config.bFreeAshUpgrade || Menu::Config.bFreeSacrificeCost || Menu::Config.bFreeStore) {
        if (dict) {
          Utils::Il2Cpp::ClearDictionary(dict);
        }
      }
      return dict;
    }

    bool (*Orig_ShopSpendingService_CanSpend)(void* __this, void* price, void* source, void* method_info);
    bool Hook_ShopSpendingService_CanSpend(void* __this, void* price, void* source, void* method_info)
    {
      if (Menu::Config.bFreeSacrificeCost || Menu::Config.bFreeAshUpgrade || Menu::Config.bFreeStore) {
        return true;
      }
      return Orig_ShopSpendingService_CanSpend(__this, price, source, method_info);
    }

    bool Hook_ShopSpendingService_Spend(void* __this, void* price, void* source, void* method_info)
    {
      if (Menu::Config.bFreeSacrificeCost || Menu::Config.bFreeAshUpgrade || Menu::Config.bFreeStore) {
        return true;
      }
      return Orig_ShopSpendingService_Spend(__this, price, source, method_info);
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
        "ShopSpendingService::CanSpend", Signatures::ShopSpendingService_CanSpend, Hook_ShopSpendingService_CanSpend,
        Orig_ShopSpendingService_CanSpend
      );
      HOOK_SIGNATURE(
        "ShopSpendingService::Spend", Signatures::ShopSpendingService_Spend, Hook_ShopSpendingService_Spend,
        Orig_ShopSpendingService_Spend
      );
      HOOK_SIGNATURE(
        "EnhancementTreeUI::Buy", Signatures::EnhancementTreeUI_Buy, Hook_EnhancementTreeUI_Buy,
        Orig_EnhancementTreeUI_Buy
      );

      HOOK_SIGNATURE(
        "MarketLotScriptableObject::CurrentPrice", Signatures::MarketLotScriptableObject_CurrentPrice,
        Hook_MarketLotScriptableObject_CurrentPrice, Orig_MarketLotScriptableObject_CurrentPrice
      );
      HOOK_SIGNATURE(
        "MarketLot::GetCurrentPrice", Signatures::MarketLot_GetCurrentPrice, Hook_MarketLot_GetCurrentPrice,
        Orig_MarketLot_GetCurrentPrice
      );

      HOOK_SIGNATURE("SacrificeUI::Buy", Signatures::SacrificeUI_Buy, Hook_SacrificeUI_Buy, Orig_SacrificeUI_Buy);
      HOOK_SIGNATURE(
        "SacrificeUI::TryRollSacrificeHalfCost", Signatures::SacrificeUI_TryRollSacrificeHalfCost,
        Hook_SacrificeUI_TryRollSacrificeHalfCost, Orig_SacrificeUI_TryRollSacrificeHalfCost
      );
    }

    void Uninitialize() { }
  }  // namespace Bonfire
}  // namespace Features
