#include "Enhancement.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace Enhancement
  {
    float (*Orig_EnhanceStationService_GetSuccessChance)(void* __this, void* itemDetail, void* method_info);

    float Hook_EnhanceStationService_GetSuccessChance(void* __this, void* itemDetail, void* method_info)
    {
      if (Menu::Config.bEnhanceItem100)
        return 1.0f;  // 100%
      return Orig_EnhanceStationService_GetSuccessChance(__this, itemDetail, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "Services.EnhanceStationService", "GetSuccessChance", 1, Hook_EnhanceStationService_GetSuccessChance,
        Orig_EnhanceStationService_GetSuccessChance
      );
    }

    void Uninitialize() { }
  }  // namespace Enhancement
}  // namespace Features
