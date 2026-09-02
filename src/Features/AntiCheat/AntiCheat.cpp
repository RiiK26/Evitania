#include "AntiCheat.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include <cstdint>

// Original function pointers
void (*Orig_AntiCheat_Initialize)(void* __this);
void (*Orig_AntiCheat_OnSpeedHackDetected)(void* __this);
void (*Orig_AntiCheat_OnObscuredCheatingDetected)(void* __this);

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

void Features::AntiCheat::Initialize()
{
  HOOK_METHOD(
    "Services.AntiCheat.AntiCheatService", "Initialize", 0, Hook_AntiCheat_Initialize, Orig_AntiCheat_Initialize
  );
  HOOK_METHOD(
    "Services.AntiCheat.AntiCheatService", "OnSpeedHackDetected", 0, Hook_AntiCheat_OnSpeedHackDetected,
    Orig_AntiCheat_OnSpeedHackDetected
  );
  HOOK_METHOD(
    "Services.AntiCheat.AntiCheatService", "OnObscuredCheatingDetected", 0, Hook_AntiCheat_OnObscuredCheatingDetected,
    Orig_AntiCheat_OnObscuredCheatingDetected
  );
}

void Features::AntiCheat::Uninitialize() { }
