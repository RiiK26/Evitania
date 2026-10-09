#include "InfiniteItems.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace InfiniteItems
  {
    bool (*Orig_BaseStorageService_Remove)(void* __this, void* item, int64_t count, void* method_info);

    bool Hook_BaseStorageService_Remove(void* __this, void* item, int64_t count, void* method_info)
    {
      if (Menu::Config.bInfiniteItems)
        return true;  // Bypass completely
      return Orig_BaseStorageService_Remove(__this, item, count, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD("BaseStorageService", "Remove", 2, Hook_BaseStorageService_Remove, Orig_BaseStorageService_Remove);
    }

    void Uninitialize() { }
  }  // namespace InfiniteItems
}  // namespace Features
