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

    void (*Orig_HourglassService_AddResource)(void* __this, void* resourceId, double delta, void* method_info);
    void Hook_HourglassService_AddResource(void* __this, void* resourceId, double delta, void* method_info)
    {
      if (Menu::Config.bInfiniteCurrency && delta < 0.0) {
        // Bypass subtraction of sand / hourglass resources
        return;
      }
      Orig_HourglassService_AddResource(__this, resourceId, delta, method_info);
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
      HOOK_SIGNATURE(
        "HourglassService::AddResource", Signatures::HourglassService_AddResource, Hook_HourglassService_AddResource,
        Orig_HourglassService_AddResource
      );
    }

    void Uninitialize() { }
  }  // namespace InfiniteCurrency
}  // namespace Features
