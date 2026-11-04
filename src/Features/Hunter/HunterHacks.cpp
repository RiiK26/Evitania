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

#include "HunterHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace HunterHacks
  {
    void (*Orig_Shop_UpdateDiscount)(void* __this, void* method_info);
    void Hook_Shop_UpdateDiscount(void* __this, void* method_info)
    {
      Orig_Shop_UpdateDiscount(__this, method_info);
      if (Menu::Config.bFreeHunterUpgrades && __this) {
        *(float*) ((uintptr_t) __this + Offsets::discount) = 0.0f;
      }
    }

    bool (*Orig_Shop_TryBuy)(void* __this, void* lot, void* method_info);
    bool Hook_Shop_TryBuy(void* __this, void* lot, void* method_info)
    {
      if (Menu::Config.bFreeHunterUpgrades && __this) {
        *(float*) ((uintptr_t) __this + Offsets::discount) = 0.0f;
      }
      return Orig_Shop_TryBuy(__this, lot, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "Shop::UpdateDiscount", Signatures::Shop_UpdateDiscount, Hook_Shop_UpdateDiscount, Orig_Shop_UpdateDiscount
      );
      HOOK_SIGNATURE("Shop::TryBuy", Signatures::Shop_TryBuy, Hook_Shop_TryBuy, Orig_Shop_TryBuy);
    }

    void Uninitialize() { }
  }  // namespace HunterHacks
}  // namespace Features
