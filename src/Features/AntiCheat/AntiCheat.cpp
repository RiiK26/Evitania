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

#if defined(_MSC_VER)
  #include <intrin.h>
  #define NOP_INSTR() __nop()
#else
  #define NOP_INSTR() __asm__ volatile("nop")
#endif

// Hook implementations
void Hook_AntiCheat_Initialize(void* __this) { NOP_INSTR(); }

void Hook_AntiCheat_OnSpeedHackDetected(void* __this)
{
  NOP_INSTR();
  NOP_INSTR();
}

void Hook_AntiCheat_OnObscuredCheatingDetected(void* __this)
{
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
}

void Hook_AntiCheat_Handle(void* __this, void* signal)
{
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
}

void Hook_AntiCheat_Apply(void* __this, void* signal)
{
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
}

void Hook_AntiCheat_ReportCheatToAnalytics(void* __this, void* signal)
{
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
  NOP_INSTR();
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
