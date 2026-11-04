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

#include "InfiniteItems.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace InfiniteItems
  {
    void (*Orig_ItemDetail_set_Amount)(void* __this, int64_t value, void* method_info);
    void Hook_ItemDetail_set_Amount(void* __this, int64_t value, void* method_info)
    {
      if (Menu::Config.bInfiniteItems && __this) {
        int64_t current_amount = *(int64_t*) ((uintptr_t) __this + Offsets::itemAmount);
        if (value < current_amount) {
          // It's a decrement (consumption), ignore it
          return;
        }
      }
      Orig_ItemDetail_set_Amount(__this, value, method_info);
    }

    bool (*Orig_BaseStorageService_Remove)(void* __this, void* item, int64_t count, void* method_info);
    bool Hook_BaseStorageService_Remove(void* __this, void* item, int64_t count, void* method_info)
    {
      if (Menu::Config.bInfiniteItems) {
        // Return true to pretend it was removed successfully without actually removing it
        return true;
      }
      return Orig_BaseStorageService_Remove(__this, item, count, method_info);
    }

    bool (*Orig_BaseStorageService_TryRemove)(void* __this, void* item, int64_t amount, void* method_info);
    bool Hook_BaseStorageService_TryRemove(void* __this, void* item, int64_t amount, void* method_info)
    {
      if (Menu::Config.bInfiniteItems) {
        // Return true to pretend it was removed successfully
        return true;
      }
      return Orig_BaseStorageService_TryRemove(__this, item, amount, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD("ItemDetail", "set_Amount", 1, Hook_ItemDetail_set_Amount, Orig_ItemDetail_set_Amount);
      HOOK_SIGNATURE(
        "BaseStorageService::Remove", Signatures::BaseStorageService_Remove, Hook_BaseStorageService_Remove,
        Orig_BaseStorageService_Remove
      );
      HOOK_SIGNATURE(
        "BaseStorageService::TryRemove", Signatures::BaseStorageService_TryRemove, Hook_BaseStorageService_TryRemove,
        Orig_BaseStorageService_TryRemove
      );
    }

    void Uninitialize() { }
  }  // namespace InfiniteItems
}  // namespace Features
