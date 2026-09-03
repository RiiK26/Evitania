#include "FastGathering.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace FastGathering
  {
    float (*Orig_GatheringService_GetSpeed)(void* __this, void* config, bool ignoreBuffs, void* method_info);

    float Hook_GatheringService_GetSpeed(void* __this, void* config, bool ignoreBuffs, void* method_info)
    {
      if (Menu::Config.bFastGathering) {
        return 100.0f;  // Very high speed multiplier
      }
      return Orig_GatheringService_GetSpeed(__this, config, ignoreBuffs, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "GatheringService::GetSpeed", Signatures::GatheringService_GetSpeed, Hook_GatheringService_GetSpeed,
        Orig_GatheringService_GetSpeed
      );
    }

    void Uninitialize() { }
  }  // namespace FastGathering
}  // namespace Features
