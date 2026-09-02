#include "ExpMultiplier.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include "../../Utils/Offsets.h"

namespace Features
{
  namespace ExpMultiplier
  {
    void (*Orig_Skill_AddExperience)(void* __this, float amount, void* method_info);

    void Hook_Skill_AddExperience(void* __this, float amount, void* method_info)
    {
      if (Menu::Config.bExpMultiplier)
        return Orig_Skill_AddExperience(__this, amount * 1000.0f, method_info);
      return Orig_Skill_AddExperience(__this, amount, method_info);
    }

    void Initialize()
    {
      HOOK_OFFSET(
        "Skill::AddExperience", Offsets::Skill_AddExperience, Hook_Skill_AddExperience, Orig_Skill_AddExperience
      );
    }

    void Uninitialize() { }
  }  // namespace ExpMultiplier
}  // namespace Features
