#pragma once
#include <cstdint>
#include "Offsets.hpp"

namespace Utils
{
  namespace Il2Cpp
  {
    // Clears a Unity Il2Cpp Dictionary<K, V> by setting its element count to 0.
    // In Unity's Il2Cpp Dictionary<K, V> implementation, the count is located at offset 0x20.
    inline void ClearDictionary(void* dict)
    {
      if (dict) {
        *(int*) ((uintptr_t) dict + Offsets::count) = 0;
      }
    }
  }  // namespace Il2Cpp
}  // namespace Utils
