#include "HunterHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace HunterHacks
  {
    void* (*Orig_MarketLot_GetCurrentPrice)(void* __this, float discount, void* method_info);
    void* Hook_MarketLot_GetCurrentPrice(void* __this, float discount, void* method_info)
    {
      void* dict = Orig_MarketLot_GetCurrentPrice(__this, discount, method_info);
      if (Menu::Config.bFreeHunterUpgrades && dict) {
        // Clear the dictionary by setting its 'count' property to 0
        // In Unity's Il2Cpp Dictionary<K, V> implementation, the count is located at offset 0x20.
        *(int*) ((uintptr_t) dict + 0x20) = 0;
      }
      return dict;
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "MarketLot::GetCurrentPrice", Signatures::MarketLot_GetCurrentPrice, Hook_MarketLot_GetCurrentPrice,
        Orig_MarketLot_GetCurrentPrice
      );
    }

    void Uninitialize() { }
  }  // namespace HunterHacks
}  // namespace Features
