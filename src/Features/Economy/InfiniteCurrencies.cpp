#include "InfiniteCurrencies.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include "../../Utils/Offsets.h"

namespace Features
{
  namespace InfiniteCurrencies
  {
    bool (*Orig_CurrencyService_Subtract)(void* __this, int currencyType, float value, void* method_info);
    void (*Orig_CurrencyService_Set1)(void* __this, int currencyType, float value, void* method_info);
    void (*Orig_CurrencyService_Set2)(void* __this, void* currency, float value, void* method_info);

    bool Hook_CurrencyService_Subtract(void* __this, int currencyType, float value, void* method_info)
    {
      if (Menu::Config.bInfiniteGold)
        value = 0.0f;  // Subtract 0
      return Orig_CurrencyService_Subtract(__this, currencyType, value, method_info);
    }

    void Hook_CurrencyService_Set1(void* __this, int currencyType, float value, void* method_info)
    {
      if (Menu::Config.bInfiniteGold) {
        if (currencyType == 0 || currencyType == 4) {  // Gold or Gem
          // Just force a high value if it drops
          value = 9999999.0f;
        }
      }
      Orig_CurrencyService_Set1(__this, currencyType, value, method_info);
    }

    void Hook_CurrencyService_Set2(void* __this, void* currency, float value, void* method_info)
    {
      if (Menu::Config.bInfiniteGold) {
        // Hard to inspect Currency object trivially, so just force high value
        value = 9999999.0f;
      }
      Orig_CurrencyService_Set2(__this, currency, value, method_info);
    }

    void Initialize()
    {
      HOOK_OFFSET(
        "CurrencyService::Subtract", Offsets::CurrencyService_Subtract, Hook_CurrencyService_Subtract,
        Orig_CurrencyService_Subtract
      );
      HOOK_OFFSET(
        "CurrencyService::Set1", Offsets::CurrencyService_Set1, Hook_CurrencyService_Set1, Orig_CurrencyService_Set1
      );
      HOOK_OFFSET(
        "CurrencyService::Set2", Offsets::CurrencyService_Set2, Hook_CurrencyService_Set2, Orig_CurrencyService_Set2
      );
    }

    void Uninitialize() { }
  }  // namespace InfiniteCurrencies
}  // namespace Features
