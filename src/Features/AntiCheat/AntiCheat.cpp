#include "AntiCheat.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"

#include <cstdint>

namespace Features
{
  namespace AntiCheat
  {
    // ─────────────────────────────────────────────────────────────
    // Layer 1: Game AntiCheatService (Services.AntiCheat)
    // Initialize is left untouched so Zenject wiring stays intact;
    // signals are dropped at Handle, and every sink is silenced.
    // ─────────────────────────────────────────────────────────────
    // void (*Orig_AntiCheat_Initialize)(void* __this, void* method);
    void (*Orig_AntiCheat_OnSpeedHackDetected)(void* __this, void* method);
    void (*Orig_AntiCheat_OnObscuredCheatingDetected)(void* __this, void* method);
    void (*Orig_AntiCheat_OnDataLoaded)(void* __this, void* method);
    void (*Orig_AntiCheat_Handle)(void* __this, void* signal, void* method);
    void (*Orig_AntiCheat_Apply)(void* __this, void* signal, void* method);
    void (*Orig_AntiCheat_ReportCheatToAnalytics)(void* __this, void* signal, void* method);
    bool (*Orig_AntiCheat_SetFlag)(void* meta, int32_t cheatType, void* method);

    // Service never subscribes to SignalBus / ACTk callbacks. let initialize
    // void Hook_AntiCheat_Initialize(void*, void*) { }

    void Hook_AntiCheat_OnSpeedHackDetected(void*, void*) { }
    void Hook_AntiCheat_OnObscuredCheatingDetected(void*, void*) { }
    void Hook_AntiCheat_OnDataLoaded(void*, void*) { }  // would flush _pendingDetections
    void Hook_AntiCheat_Handle(void*, void*, void*) { }
    void Hook_AntiCheat_Apply(void*, void*, void*) { }
    void Hook_AntiCheat_ReportCheatToAnalytics(void*, void*, void*) { }

    // static bool SetFlag(MetaModel, CheatType) -> never dirty save flags.
    bool Hook_AntiCheat_SetFlag(void*, int32_t, void*) { return false; }

    // ─────────────────────────────────────────────────────────────
    // Layer 2: Persistent player state / save integrity
    // ─────────────────────────────────────────────────────────────
    bool (*Orig_MetaModel_get_IsCheater)(void* __this, void* method);
    void (*Orig_ObscuredFilePrefs_OnNotGenuineDataDetected)(void* method);
    void (*Orig_ObscuredFilePrefs_OnDataFromAnotherDeviceDetected)(void* method);
    void (*Orig_PlayerData_FireAccountMismatchAnalytics)(
      void* __this, void* sessionPlayFabId, void* cloudModel, void* method
    );

    // Safety net: clean account even if the save already carries flags.
    bool Hook_MetaModel_get_IsCheater(void*, void*) { return false; }

    // Encrypted prefs tamper / device-lock callbacks (source of SaveAltered / ForeignSave).
    void Hook_ObscuredFilePrefs_OnNotGenuineDataDetected(void*) { }
    void Hook_ObscuredFilePrefs_OnDataFromAnotherDeviceDetected(void*) { }

    // Local-vs-cloud account mismatch telemetry.
    void Hook_PlayerData_FireAccountMismatchAnalytics(void*, void*, void*, void*) { }

    // ─────────────────────────────────────────────────────────────
    // Layer 3: ACTk detectors (CodeStage.AntiCheat.Detectors)
    // ─────────────────────────────────────────────────────────────
    // Shared generic base (ACTkDetectorBase<__Canon>): one address serves every
    // detector -> covers Injection / WallHack / SpeedHack / Obscured at once.
    void (*Orig_ACTkBase_OnCheatingDetected)(void* __this, void* detectionInfo, void* method);
    void (*Orig_ACTkBase_InvokeCheatingDetectedEvent)(void* __this, void* method);

    void Hook_ACTkBase_OnCheatingDetected(void*, void*, void*) { }
    void Hook_ACTkBase_InvokeCheatingDetectedEvent(void*, void*) { }

    bool (*Orig_SpeedHack_IsTicksCheated)(
      void* __this, int64_t ticks, int64_t* previousTicks, int64_t reliableDelta, void* method
    );
    bool (*Orig_SpeedHack_IsTimeScaleCheated)(void* __this, void* method);
    void (*Orig_Obscured_OnCheatingDetected)(void* __this, void* type, void* decrypted, void* fake, void* method);
    void (*Orig_Obscured_OnCheatingDetected_1)(
      void* __this, void* type, bool hashValid, void* decrypted, void* fake, void* method
    );
    void (*Orig_TimeCheating_ReportCheckResult)(void* __this, void* method);

    // Run original so internal tick/timescale bookkeeping stays consistent,
    // then report "clean" so false-positive counters never accumulate.
    bool Hook_SpeedHack_IsTicksCheated(
      void* __this, int64_t ticks, int64_t* previousTicks, int64_t reliableDelta, void* method
    )
    {
      Orig_SpeedHack_IsTicksCheated(__this, ticks, previousTicks, reliableDelta, method);
      return false;
    }

    bool Hook_SpeedHack_IsTimeScaleCheated(void* __this, void* method)
    {
      Orig_SpeedHack_IsTimeScaleCheated(__this, method);
      return false;
    }

    // Obscured detector has its own non-virtual entry points before the base call.
    void Hook_Obscured_OnCheatingDetected(void*, void*, void*, void*, void*) { }
    void Hook_Obscured_OnCheatingDetected_1(void*, void*, bool, void*, void*, void*) { }

    // TimeCheatingDetector fires its own CheatChecked event, bypassing the base.
    void Hook_TimeCheating_ReportCheckResult(void*, void*) { }

    // ─────────────────────────────────────────────────────────────
    // Policy: native kill-switch for consequences (set by name, not offset).
    // ─────────────────────────────────────────────────────────────
    void DisableConsequencesPolicy()
    {
      bool value = false;
      IL2CPP::Class::Utils::SetStaticField("Services.AntiCheat.AntiCheatPolicy", "ApplyConsequences", &value);
    }

    void Initialize()
    {
      // Layer 1
      HOOK_SIGNATURE(
        "AntiCheatService::OnSpeedHackDetected", Signatures::AntiCheatService_OnSpeedHackDetected,
        Hook_AntiCheat_OnSpeedHackDetected, Orig_AntiCheat_OnSpeedHackDetected
      );
      HOOK_SIGNATURE(
        "AntiCheatService::OnObscuredCheatingDetected", Signatures::AntiCheatService_OnObscuredCheatingDetected,
        Hook_AntiCheat_OnObscuredCheatingDetected, Orig_AntiCheat_OnObscuredCheatingDetected
      );
      HOOK_SIGNATURE(
        "AntiCheatService::OnDataLoaded", Signatures::AntiCheatService_OnDataLoaded, Hook_AntiCheat_OnDataLoaded,
        Orig_AntiCheat_OnDataLoaded
      );
      HOOK_SIGNATURE(
        "AntiCheatService::Handle", Signatures::AntiCheatService_Handle, Hook_AntiCheat_Handle, Orig_AntiCheat_Handle
      );
      HOOK_SIGNATURE(
        "AntiCheatService::Apply", Signatures::AntiCheatService_Apply, Hook_AntiCheat_Apply, Orig_AntiCheat_Apply
      );
      HOOK_SIGNATURE(
        "AntiCheatService::ReportCheatToAnalytics", Signatures::AntiCheatService_ReportCheatToAnalytics,
        Hook_AntiCheat_ReportCheatToAnalytics, Orig_AntiCheat_ReportCheatToAnalytics
      );
      HOOK_SIGNATURE(
        "AntiCheatService::SetFlag", Signatures::AntiCheatService_SetFlag, Hook_AntiCheat_SetFlag,
        Orig_AntiCheat_SetFlag
      );

      // Layer 2
      HOOK_SIGNATURE(
        "MetaModel::get_IsCheater", Signatures::MetaModel_get_IsCheater, Hook_MetaModel_get_IsCheater,
        Orig_MetaModel_get_IsCheater
      );
      HOOK_SIGNATURE(
        "ObscuredFilePrefs::OnNotGenuineDataDetected", Signatures::ObscuredFilePrefs_OnNotGenuineDataDetected,
        Hook_ObscuredFilePrefs_OnNotGenuineDataDetected, Orig_ObscuredFilePrefs_OnNotGenuineDataDetected
      );
      HOOK_SIGNATURE(
        "ObscuredFilePrefs::OnDataFromAnotherDeviceDetected",
        Signatures::ObscuredFilePrefs_OnDataFromAnotherDeviceDetected,
        Hook_ObscuredFilePrefs_OnDataFromAnotherDeviceDetected, Orig_ObscuredFilePrefs_OnDataFromAnotherDeviceDetected
      );
      HOOK_SIGNATURE(
        "PlayerDataStorageService::FireAccountMismatchAnalytics",
        Signatures::PlayerDataStorageService_FireAccountMismatchAnalytics, Hook_PlayerData_FireAccountMismatchAnalytics,
        Orig_PlayerData_FireAccountMismatchAnalytics
      );

      // Layer 3
      HOOK_SIGNATURE(
        "ACTkDetectorBase::OnCheatingDetected", Signatures::ACTkDetectorBase_OnCheatingDetected,
        Hook_ACTkBase_OnCheatingDetected, Orig_ACTkBase_OnCheatingDetected
      );
      HOOK_SIGNATURE(
        "ACTkDetectorBase::InvokeCheatingDetectedEvent", Signatures::ACTkDetectorBase_InvokeCheatingDetectedEvent,
        Hook_ACTkBase_InvokeCheatingDetectedEvent, Orig_ACTkBase_InvokeCheatingDetectedEvent
      );
      HOOK_SIGNATURE(
        "SpeedHackDetector::IsTicksCheated", Signatures::SpeedHackDetector_IsTicksCheated,
        Hook_SpeedHack_IsTicksCheated, Orig_SpeedHack_IsTicksCheated
      );
      HOOK_SIGNATURE(
        "SpeedHackDetector::IsTimeScaleCheated", Signatures::SpeedHackDetector_IsTimeScaleCheated,
        Hook_SpeedHack_IsTimeScaleCheated, Orig_SpeedHack_IsTimeScaleCheated
      );
      HOOK_SIGNATURE(
        "ObscuredCheatingDetector::OnCheatingDetected", Signatures::ObscuredCheatingDetector_OnCheatingDetected,
        Hook_Obscured_OnCheatingDetected, Orig_Obscured_OnCheatingDetected
      );
      HOOK_SIGNATURE(
        "ObscuredCheatingDetector::OnCheatingDetected_1", Signatures::ObscuredCheatingDetector_OnCheatingDetected_1,
        Hook_Obscured_OnCheatingDetected_1, Orig_Obscured_OnCheatingDetected_1
      );
      HOOK_SIGNATURE(
        "TimeCheatingDetector::ReportCheckResult", Signatures::TimeCheatingDetector_ReportCheckResult,
        Hook_TimeCheating_ReportCheckResult, Orig_TimeCheating_ReportCheckResult
      );

      // Policy
      DisableConsequencesPolicy();
    }

    void Uninitialize() { }
  }  // namespace AntiCheat
}  // namespace Features
