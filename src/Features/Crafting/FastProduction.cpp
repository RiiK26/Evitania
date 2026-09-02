#include "FastProduction.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace FastProduction
  {
    float (*Orig_BaseProductionProcessorService_get_SpeedModifier)(void* __this, void* method_info);

    float Hook_BaseProductionProcessorService_get_SpeedModifier(void* __this, void* method_info)
    {
      // Extremely fast production/smelting
      // This isn't currently toggleable in the menu under its own checkbox,
      // but it was bundled with Free Crafting / Smelting previously.
      if (Menu::Config.bFreeCrafting)
        return 999.0f;
      return Orig_BaseProductionProcessorService_get_SpeedModifier(__this, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "BaseProductionProcessorService", "get_SpeedModifier", 0, Hook_BaseProductionProcessorService_get_SpeedModifier,
        Orig_BaseProductionProcessorService_get_SpeedModifier
      );
    }

    void Uninitialize() { }
  }  // namespace FastProduction
}  // namespace Features
