#include "FastTimeLine.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace FastTimeLine
  {
    double (*Orig_HourglassService_PrestigeSpeedMult)(void* __this, void* method_info);
    double Hook_HourglassService_PrestigeSpeedMult(void* __this, void* method_info)
    {
      double res = Orig_HourglassService_PrestigeSpeedMult(__this, method_info);
      if (Menu::Config.bFastTimeLine) {
        res *= Menu::Config.fTimeLineMultiplier;
      }
      return res;
    }

    double (*Orig_HourglassService_PrestigeCycleIncome)(void* __this, void* method_info);
    double Hook_HourglassService_PrestigeCycleIncome(void* __this, void* method_info)
    {
      double res = Orig_HourglassService_PrestigeCycleIncome(__this, method_info);
      if (Menu::Config.bFastTimeLine) {
        res *= Menu::Config.fTimeLineMultiplier;
      }
      return res;
    }

    double (*Orig_HourglassService_TotalRate)(void* __this, void* method_info);
    double Hook_HourglassService_TotalRate(void* __this, void* method_info)
    {
      double res = Orig_HourglassService_TotalRate(__this, method_info);
      if (Menu::Config.bFastTimeLine) {
        res *= Menu::Config.fTimeLineMultiplier;
      }
      return res;
    }

    double (*Orig_HourglassService_PrestigeRatePerSecond)(void* __this, void* method_info);
    double Hook_HourglassService_PrestigeRatePerSecond(void* __this, void* method_info)
    {
      double res = Orig_HourglassService_PrestigeRatePerSecond(__this, method_info);
      if (Menu::Config.bFastTimeLine) {
        res *= Menu::Config.fTimeLineMultiplier;
      }
      return res;
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "HourglassService::PrestigeSpeedMult", Signatures::HourglassService_PrestigeSpeedMult,
        Hook_HourglassService_PrestigeSpeedMult, Orig_HourglassService_PrestigeSpeedMult
      );
      HOOK_SIGNATURE(
        "HourglassService::PrestigeCycleIncome", Signatures::HourglassService_PrestigeCycleIncome,
        Hook_HourglassService_PrestigeCycleIncome, Orig_HourglassService_PrestigeCycleIncome
      );
      HOOK_SIGNATURE(
        "HourglassService::TotalRate", Signatures::HourglassService_TotalRate, Hook_HourglassService_TotalRate,
        Orig_HourglassService_TotalRate
      );
      HOOK_SIGNATURE(
        "HourglassService::PrestigeRatePerSecond", Signatures::HourglassService_PrestigeRatePerSecond,
        Hook_HourglassService_PrestigeRatePerSecond, Orig_HourglassService_PrestigeRatePerSecond
      );
    }

    void Uninitialize() { }
  }  // namespace FastTimeLine

}  // namespace Features
