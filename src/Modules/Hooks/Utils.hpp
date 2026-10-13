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
