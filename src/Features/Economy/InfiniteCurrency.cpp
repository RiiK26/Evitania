#include "InfiniteCurrency.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace InfiniteCurrency
  {
    bool (*Orig_CurrencyService_Subtract)(void* __this, uint32_t currencyType, float value, void* method_info);
    bool Hook_CurrencyService_Subtract(void* __this, uint32_t currencyType, float value, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency) {
        return true;  // Bypass subtraction
      }
      return Orig_CurrencyService_Subtract(__this, currencyType, value, method_info);
    }

    bool (*Orig_CurrencyService_Subtract_1)(void* __this, void* currencyObj, float value, void* method_info);
    bool Hook_CurrencyService_Subtract_1(void* __this, void* currencyObj, float value, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency) {
        return true;  // Bypass subtraction
      }
      return Orig_CurrencyService_Subtract_1(__this, currencyObj, value, method_info);
    }

    // Infinite sand
    bool bIsPurchasing = false;

    double (*Orig_HourglassService_CostFactor)(void* __this, void* genSpec, void* method_info);
    double Hook_HourglassService_CostFactor(void* __this, void* genSpec, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0.0;
      return Orig_HourglassService_CostFactor(__this, genSpec, method_info);
    }

    double (*Orig_HourglassService_UpgradeCost)(void* __this, void* id, void* method_info);
    double Hook_HourglassService_UpgradeCost(void* __this, void* id, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0.0;
      return Orig_HourglassService_UpgradeCost(__this, id, method_info);
    }

    double (*Orig_HourglassService_LevelCost)(void* __this, void* method_info);
    double Hook_HourglassService_LevelCost(void* __this, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0.0;
      return Orig_HourglassService_LevelCost(__this, method_info);
    }

    double (*Orig_HourglassTalentConfig_get_PrestigeCost)(void* __this, void* method_info);
    double Hook_HourglassTalentConfig_get_PrestigeCost(void* __this, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0.0;
      return Orig_HourglassTalentConfig_get_PrestigeCost(__this, method_info);
    }

    double (*Orig_HourglassUpgradeItem_GetCost)(void* __this, int currentLevel, void* method_info);
    double Hook_HourglassUpgradeItem_GetCost(void* __this, int currentLevel, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0.0;
      return Orig_HourglassUpgradeItem_GetCost(__this, currentLevel, method_info);
    }

    long (*Orig_HourglassHardUUpgrade_GetCostAmount)(void* __this, int nextLevel, void* method_info);
    long Hook_HourglassHardUUpgrade_GetCostAmount(void* __this, int nextLevel, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && bIsPurchasing)
        return 0;
      return Orig_HourglassHardUUpgrade_GetCostAmount(__this, nextLevel, method_info);
    }

    bool (*Orig_HourglassService_BuyUpgrade)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyUpgrade(void* __this, void* id, void* method_info)
    {
      bIsPurchasing = true;
      bool res      = Orig_HourglassService_BuyUpgrade(__this, id, method_info);
      bIsPurchasing = false;
      return res;
    }

    bool (*Orig_HourglassService_BuyHardUpgrade)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyHardUpgrade(void* __this, void* id, void* method_info)
    {
      bIsPurchasing = true;
      bool res      = Orig_HourglassService_BuyHardUpgrade(__this, id, method_info);
      bIsPurchasing = false;
      return res;
    }

    bool (*Orig_HourglassService_BuyTalent)(void* __this, void* id, void* method_info);
    bool Hook_HourglassService_BuyTalent(void* __this, void* id, void* method_info)
    {
      bIsPurchasing = true;
      bool res      = Orig_HourglassService_BuyTalent(__this, id, method_info);
      bIsPurchasing = false;
      return res;
    }

    bool (*Orig_HourglassService_BuyGenerator)(void* __this, void* id, long amount, void* method_info);
    bool Hook_HourglassService_BuyGenerator(void* __this, void* id, long amount, void* method_info)
    {
      bIsPurchasing = true;
      bool res      = Orig_HourglassService_BuyGenerator(__this, id, amount, method_info);
      bIsPurchasing = false;
      return res;
    }

    void (*Orig_HourglassService_BuyUpgradeBlock)(void* __this, void* block, void* method_info);
    void Hook_HourglassService_BuyUpgradeBlock(void* __this, void* block, void* method_info)
    {
      bIsPurchasing = true;
      Orig_HourglassService_BuyUpgradeBlock(__this, block, method_info);
      bIsPurchasing = false;
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "CurrencyService::Subtract(CurrencyType)", Signatures::CurrencyService_Subtract, Hook_CurrencyService_Subtract,
        Orig_CurrencyService_Subtract
      );
      HOOK_SIGNATURE(
        "CurrencyService::Subtract(Currency)", Signatures::CurrencyService_Subtract_1, Hook_CurrencyService_Subtract_1,
        Orig_CurrencyService_Subtract_1
      );
      // Hourglass bypass for infinite sand buying upgrades
      HOOK_SIGNATURE(
        "HourglassService::CostFactor", Signatures::HourglassService_CostFactor, Hook_HourglassService_CostFactor,
        Orig_HourglassService_CostFactor
      );
      HOOK_SIGNATURE(
        "HourglassService::UpgradeCost", Signatures::HourglassService_UpgradeCost, Hook_HourglassService_UpgradeCost,
        Orig_HourglassService_UpgradeCost
      );
      HOOK_SIGNATURE(
        "HourglassService::LevelCost", Signatures::HourglassService_LevelCost, Hook_HourglassService_LevelCost,
        Orig_HourglassService_LevelCost
      );
      HOOK_METHOD(
        "GameSystems.Hourglass.Config.Prestige.HourglassTalentConfig", "get_PrestigeCost", 0,
        Hook_HourglassTalentConfig_get_PrestigeCost, Orig_HourglassTalentConfig_get_PrestigeCost
      );
      HOOK_SIGNATURE(
        "HourglassUpgradeItem::GetCost", Signatures::HourglassUpgradeItem_GetCost, Hook_HourglassUpgradeItem_GetCost,
        Orig_HourglassUpgradeItem_GetCost
      );
      HOOK_SIGNATURE(
        "HourglassHardUUpgrade::GetCostAmount", Signatures::HourglassHardUUpgrade_GetCostAmount,
        Hook_HourglassHardUUpgrade_GetCostAmount, Orig_HourglassHardUUpgrade_GetCostAmount
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyUpgrade", Signatures::HourglassService_BuyUpgrade, Hook_HourglassService_BuyUpgrade,
        Orig_HourglassService_BuyUpgrade
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyHardUpgrade", Signatures::HourglassService_BuyHardUpgrade,
        Hook_HourglassService_BuyHardUpgrade, Orig_HourglassService_BuyHardUpgrade
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyTalent", Signatures::HourglassService_BuyTalent, Hook_HourglassService_BuyTalent,
        Orig_HourglassService_BuyTalent
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyGenerator", Signatures::HourglassService_BuyGenerator, Hook_HourglassService_BuyGenerator,
        Orig_HourglassService_BuyGenerator
      );
      HOOK_SIGNATURE(
        "HourglassService::BuyUpgradeBlock", Signatures::HourglassService_BuyUpgradeBlock,
        Hook_HourglassService_BuyUpgradeBlock, Orig_HourglassService_BuyUpgradeBlock
      );
    }

    void Uninitialize() { }
  }  // namespace InfiniteCurrency
}  // namespace Features
