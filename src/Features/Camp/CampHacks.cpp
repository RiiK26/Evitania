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

#include "CampHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

#include <cstdint>

namespace Features
{
  namespace CampHacks
  {
    namespace
    {
      constexpr int64_t kVirtualBagStock = 999999999LL;

      int64_t (*Orig_ClientCampBag_Count)(void* __this, void* resId, void* method_info);
      bool (*Orig_ClientCampBag_TrySpend)(void* __this, void* items, void* method_info);

      int64_t Hook_ClientCampBag_Count(void* __this, void* resId, void* method_info)
      {
        if (Menu::Config.bFreeCampBag) {
          return kVirtualBagStock;
        }
        return Orig_ClientCampBag_Count(__this, resId, method_info);
      }

      bool Hook_ClientCampBag_TrySpend(void* __this, void* items, void* method_info)
      {
        if (Menu::Config.bFreeCampBag) {
          return true;
        }
        return Orig_ClientCampBag_TrySpend(__this, items, method_info);
      }
    }  // namespace

    void Initialize()
    {
      HOOK_SIGNATURE(
        "ClientCampBag::Count", Signatures::ClientCampBag_Count, Hook_ClientCampBag_Count, Orig_ClientCampBag_Count
      );
      HOOK_SIGNATURE(
        "ClientCampBag::TrySpend", Signatures::ClientCampBag_TrySpend, Hook_ClientCampBag_TrySpend,
        Orig_ClientCampBag_TrySpend
      );
    }

    void Uninitialize() { }
  }  // namespace CampHacks
}  // namespace Features
