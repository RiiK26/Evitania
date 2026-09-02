#include "DiscountedVendor.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace DiscountedVendor
  {
    int (*Orig_ShopMarketLotDescription_get_Cost)(void* __this, void* method_info);

    int Hook_ShopMarketLotDescription_get_Cost(void* __this, void* method_info)
    {
      if (Menu::Config.bDiscountedVendor)
        return 0;
      return Orig_ShopMarketLotDescription_get_Cost(__this, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "ShopMarketLotDescription", "get_Cost", 0, Hook_ShopMarketLotDescription_get_Cost,
        Orig_ShopMarketLotDescription_get_Cost
      );
    }

    void Uninitialize() { }
  }  // namespace DiscountedVendor
}  // namespace Features
