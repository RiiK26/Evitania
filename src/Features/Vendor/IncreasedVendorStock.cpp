#include "IncreasedVendorStock.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace IncreasedVendorStock
  {
    long (*Orig_VendorService_GetBoughtCount)(void* __this, void* vendorName, void* itemGuid, void* method_info);

    long Hook_VendorService_GetBoughtCount(void* __this, void* vendorName, void* itemGuid, void* method_info)
    {
      if (Menu::Config.bIncreasedVendorStock)
        return 0;
      return Orig_VendorService_GetBoughtCount(__this, vendorName, itemGuid, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "VendorService", "GetBoughtCount", 2, Hook_VendorService_GetBoughtCount, Orig_VendorService_GetBoughtCount
      );
    }

    void Uninitialize() { }
  }  // namespace IncreasedVendorStock
}  // namespace Features
