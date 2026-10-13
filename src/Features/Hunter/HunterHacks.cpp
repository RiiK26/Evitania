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
