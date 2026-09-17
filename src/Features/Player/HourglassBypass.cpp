#include "HourglassBypass.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace HourglassBypass
  {
    bool (*Orig_TimeskipItem_get_CanUseImpl)(void* __this, void* method_info);

    bool Hook_TimeskipItem_get_CanUseImpl(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassBypass) {
        return true;
      }
      return Orig_TimeskipItem_get_CanUseImpl(__this, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "TimeskipItem::get_CanUseImpl", Signatures::TimeskipItem_get_CanUseImpl, Hook_TimeskipItem_get_CanUseImpl,
        Orig_TimeskipItem_get_CanUseImpl
      );
    }

    void Uninitialize() { }
  }  // namespace HourglassBypass
}  // namespace Features
