#include "ExpMultiplier.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace ExpMultiplier
  {
    void (*Orig_Skill_AddExperience)(void* __this, float amount, void* method_info);

    void Hook_Skill_AddExperience(void* __this, float amount, void* method_info)
    {
      if (Menu::Config.bExpMultiplier) {
        amount *= Menu::Config.fExpMultiplierValue;  // Multiply experience
      }
      Orig_Skill_AddExperience(__this, amount, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "Skill::AddExperience", Signatures::Skill_AddExperience, Hook_Skill_AddExperience, Orig_Skill_AddExperience
      );
    }

    void Uninitialize() { }
  }  // namespace ExpMultiplier
}  // namespace Features
