#include "SpeedHack.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace SpeedHack
  {
    void (*Orig_Time_set_timeScale)(float value, void* method_info);

    void Hook_Time_set_timeScale(float value, void* method_info)
    {
      if (Menu::Config.bSpeedHack) {
        value = Menu::Config.fSpeedMultiplier;
      }
      Orig_Time_set_timeScale(value, method_info);
    }

    void ApplySpeedHack()
    {
      if (Orig_Time_set_timeScale) {
        // Calling it with 1.0f will trigger our hook to multiply it by fSpeedMultiplier if enabled.
        Hook_Time_set_timeScale(1.0f, nullptr);
      }
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "Time::set_timeScale", Signatures::Time_timeScale, Hook_Time_set_timeScale, Orig_Time_set_timeScale
      );
    }

    void Uninitialize() { }
  }  // namespace SpeedHack
}  // namespace Features
