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

#pragma once
#include <cstdint>
#include "Offsets.hpp"

namespace Utils
{
  namespace Il2Cpp
  {
    // Clears a Unity Il2Cpp Dictionary<K, V> by setting its element count to 0.
    inline void ClearDictionary(void* dict)
    {
      if (dict) {
        *(int*) ((uintptr_t) dict + Offsets::Il2Cpp::Dictionary::count) = 0;
      }
    }

    // Sets all float values in a Unity Il2Cpp Dictionary<TKey, float> to 0.0f
    // while keeping keys intact so UI elements (icons, sprites) still display.
    inline void ZeroDictionaryFloatValues(void* dict)
    {
      if (!dict)
        return;

      int   count        = *(int*) ((uintptr_t) dict + Offsets::Il2Cpp::Dictionary::count);
      void* entriesArray = *(void**) ((uintptr_t) dict + Offsets::Il2Cpp::Dictionary::entries);
      if (!entriesArray || count <= 0)
        return;

      int maxLen = *(int*) ((uintptr_t) entriesArray + Offsets::Il2Cpp::Array::length);
      if (count > maxLen) {
        count = maxLen;
      }

      uintptr_t entryPtr = (uintptr_t) entriesArray + Offsets::Il2Cpp::Array::data;
      for (int i = 0; i < count; ++i) {
        uintptr_t currentEntry = entryPtr + (i * Offsets::Il2Cpp::Dictionary::entryStride);
        int32_t   hashCode     = *(int32_t*) currentEntry;
        if (hashCode >= 0) {
          *(float*) (currentEntry + Offsets::Il2Cpp::Dictionary::valueOffset) = 0.0f;
        }
      }
    }
  }  // namespace Il2Cpp
}  // namespace Utils
