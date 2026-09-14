#include "AntiCheat.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"

// Original function pointers
void (*Orig_AntiCheat_Initialize)(void* __this);
void (*Orig_AntiCheat_OnSpeedHackDetected)(void* __this);
void (*Orig_AntiCheat_OnObscuredCheatingDetected)(void* __this);
void (*Orig_AntiCheat_Handle)(void* __this, void* signal);
void (*Orig_AntiCheat_Apply)(void* __this, void* signal);
void (*Orig_AntiCheat_ReportCheatToAnalytics)(void* __this, void* signal);

// Hook implementations
void Hook_AntiCheat_Initialize(void* __this)
{
  // Bypass initialization
}

void Hook_AntiCheat_OnSpeedHackDetected(void* __this)
{
  // Do nothing
}

void Hook_AntiCheat_OnObscuredCheatingDetected(void* __this)
{
  // Do nothing
}

void Hook_AntiCheat_Handle(void* __this, void* signal)
{
  // Do nothing
}

void Hook_AntiCheat_Apply(void* __this, void* signal)
{
  // Do nothing
}

void Hook_AntiCheat_ReportCheatToAnalytics(void* __this, void* signal)
{
  // Do nothing
}

void Features::AntiCheat::Initialize()
{
  HOOK_SIGNATURE(
    "AntiCheatService::Initialize", Signatures::AntiCheatService_Initialize, Hook_AntiCheat_Initialize,
    Orig_AntiCheat_Initialize
  );
  HOOK_SIGNATURE(
    "AntiCheatService::OnSpeedHackDetected", Signatures::AntiCheatService_OnSpeedHackDetected,
    Hook_AntiCheat_OnSpeedHackDetected, Orig_AntiCheat_OnSpeedHackDetected
  );
  HOOK_SIGNATURE(
    "AntiCheatService::OnObscuredCheatingDetected", Signatures::AntiCheatService_OnObscuredCheatingDetected,
    Hook_AntiCheat_OnObscuredCheatingDetected, Orig_AntiCheat_OnObscuredCheatingDetected
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
}

void Features::AntiCheat::Uninitialize() { }
