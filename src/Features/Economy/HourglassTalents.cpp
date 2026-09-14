#include "HourglassTalents.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace HourglassTalents
  {
    bool (*Orig_HourglassService_CanBuyTalent)(void* __this, void* talentId, void* method_info);
    bool Hook_HourglassService_CanBuyTalent(void* __this, void* talentId, void* method_info)
    {
      if (Menu::Config.bHourglassFreeTalents) {
        return true;
      }
      return Orig_HourglassService_CanBuyTalent(__this, talentId, method_info);
    }

    bool (*Orig_HourglassService_IsTalentUnlocked)(void* __this, void* talentId, void* method_info);
    bool Hook_HourglassService_IsTalentUnlocked(void* __this, void* talentId, void* method_info)
    {
      if (Menu::Config.bHourglassUnlockAllTalents) {
        return true;
      }
      return Orig_HourglassService_IsTalentUnlocked(__this, talentId, method_info);
    }

    int (*Orig_HourglassService_GetTalentLevel)(void* __this, void* talentId, void* method_info);
    int Hook_HourglassService_GetTalentLevel(void* __this, void* talentId, void* method_info)
    {
      if (Menu::Config.bHourglassUnlockAllTalents) {
        return 1;  // max lvl just need 1
      }
      return Orig_HourglassService_GetTalentLevel(__this, talentId, method_info);
    }

    double (*Orig_HourglassService_PreviewRemortReward)(void* __this, void* method_info);
    double Hook_HourglassService_PreviewRemortReward(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassMassiveRemort) {
        return 999999999999.0;
      }
      return Orig_HourglassService_PreviewRemortReward(__this, method_info);
    }

    double (*Orig_HourglassService_RunEarned)(void* __this, void* method_info);
    double Hook_HourglassService_RunEarned(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassMassiveRemort) {
        return 999999999999.0;
      }
      return Orig_HourglassService_RunEarned(__this, method_info);
    }

    double (*Orig_HourglassTalentConfig_get_PrestigeCost)(void* __this, void* method_info);
    double Hook_HourglassTalentConfig_get_PrestigeCost(void* __this, void* method_info)
    {
      if (Menu::Config.bHourglassFreeTalents) {
        return 0.0;
      }
      return Orig_HourglassTalentConfig_get_PrestigeCost(__this, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "HourglassService::CanBuyTalent", Signatures::HourglassService_CanBuyTalent, Hook_HourglassService_CanBuyTalent,
        Orig_HourglassService_CanBuyTalent
      );
      HOOK_SIGNATURE(
        "HourglassService::IsTalentUnlocked", Signatures::HourglassService_IsTalentUnlocked,
        Hook_HourglassService_IsTalentUnlocked, Orig_HourglassService_IsTalentUnlocked
      );
      HOOK_SIGNATURE(
        "HourglassService::GetTalentLevel", Signatures::HourglassService_GetTalentLevel,
        Hook_HourglassService_GetTalentLevel, Orig_HourglassService_GetTalentLevel
      );
      HOOK_SIGNATURE(
        "HourglassService::PreviewRemortReward", Signatures::HourglassService_PreviewRemortReward,
        Hook_HourglassService_PreviewRemortReward, Orig_HourglassService_PreviewRemortReward
      );
      HOOK_SIGNATURE(
        "HourglassService::RunEarned", Signatures::HourglassService_RunEarned, Hook_HourglassService_RunEarned,
        Orig_HourglassService_RunEarned
      );
      HOOK_SIGNATURE(
        "HourglassTalentConfig::get_PrestigeCost", Signatures::HourglassTalentConfig_PrestigeCost,
        Hook_HourglassTalentConfig_get_PrestigeCost, Orig_HourglassTalentConfig_get_PrestigeCost
      );
    }

    void Uninitialize() { }
  }  // namespace HourglassTalents
}  // namespace Features
