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
    }

    void Uninitialize() { }
  }  // namespace InfiniteCurrency
}  // namespace Features
