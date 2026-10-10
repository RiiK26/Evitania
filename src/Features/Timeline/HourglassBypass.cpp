#include "HourglassBypass.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

#include <unordered_map>

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

    int32_t (*Orig_HourglassTalentConfig_get_Branch)(void* __this, void* method_info);

    std::unordered_map<void*, double> originalTalentCosts;

    double (*Orig_HourglassTalentConfig_get_Cost)(void* __this, void* method_info);
    double Hook_HourglassTalentConfig_get_Cost(void* __this, void* method_info)
    {
      if (__this) {
        double* pCost = (double*) ((uintptr_t) __this + Offsets::cost);

        if (Menu::Config.bFreeTimelineNodes || Menu::Config.bFreeRestorationNodes) {
          if (*pCost != 0.0) {
            originalTalentCosts[__this] = *pCost;
            *pCost                      = 0.0;
          }
        }
        else {
          if (*pCost == 0.0) {
            auto it = originalTalentCosts.find(__this);
            if (it != originalTalentCosts.end()) {
              *pCost = it->second;
            }
          }
        }
      }
      return Orig_HourglassTalentConfig_get_Cost(__this, method_info);
    }

    void* (*Orig_HourglassSystemUI_BuildTalentTooltipContent)(void* __this, void* talent, void* method_info);
    void* Hook_HourglassSystemUI_BuildTalentTooltipContent(void* __this, void* talent, void* method_info)
    {
      if (talent) {
        double* pCost = (double*) ((uintptr_t) talent + Offsets::cost);

        if (Menu::Config.bFreeTimelineNodes || Menu::Config.bFreeRestorationNodes) {
          if (*pCost != 0.0) {
            originalTalentCosts[talent] = *pCost;
            *pCost                      = 0.0;
          }
        }
        else {
          if (*pCost == 0.0) {
            auto it = originalTalentCosts.find(talent);
            if (it != originalTalentCosts.end()) {
              *pCost = it->second;
            }
          }
        }
      }
      return Orig_HourglassSystemUI_BuildTalentTooltipContent(__this, talent, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "TimeskipItem::get_CanUseImpl", Signatures::TimeskipItem_get_CanUseImpl, Hook_TimeskipItem_get_CanUseImpl,
        Orig_TimeskipItem_get_CanUseImpl
      );

      HOOK_SIGNATURE(
        "HourglassTalentConfig::get_Cost", Signatures::HourglassTalentConfig_get_Cost,
        Hook_HourglassTalentConfig_get_Cost, Orig_HourglassTalentConfig_get_Cost
      );

      HOOK_SIGNATURE(
        "HourglassSystemUI::BuildTalentTooltipContent", Signatures::HourglassSystemUI_BuildTalentTooltipContent,
        Hook_HourglassSystemUI_BuildTalentTooltipContent, Orig_HourglassSystemUI_BuildTalentTooltipContent
      );
    }

    void Uninitialize() { }
  }  // namespace HourglassBypass
}  // namespace Features
