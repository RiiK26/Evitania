#include "Hooks.hpp"

#include "../../Features/Combat/GodMode.hpp"
#include "../../Features/Combat/AuraKill.hpp"
#include "../../Features/Player/ExpMultiplier.hpp"
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/Player/Enhancement.hpp"
#include "../../Features/Economy/InfiniteCurrency.hpp"
#include "../../Features/Economy/FreeStore.hpp"
#include "../../Features/Player/SpeedHack.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"
#include "../../Features/Player/HourglassBypass.hpp"
#include "../../Features/Curio/CurioHacks.hpp"
#include "../../Features/Engineer/EngineerHacks.hpp"
#include "../../Features/Hunter/HunterHacks.hpp"
#include "../../Features/Bonfire/Bonfire.hpp"

#include "MinHook.h"

bool Hooks::bHooksFailed = false;

void Hooks::Initialize()
{
  int mhStatus = MH_Initialize();
  if (mhStatus != MH_OK && mhStatus != MH_ERROR_ALREADY_INITIALIZED) {
    Hooks::bHooksFailed = true;
  }

  // Initialize modular features
  Features::GodMode::Initialize();
  Features::AuraKill::Initialize();
  Features::ExpMultiplier::Initialize();
  Features::InfiniteItems::Initialize();
  Features::Enhancement::Initialize();
  Features::InfiniteCurrency::Initialize();
  Features::FreeStore::Initialize();
  Features::SpeedHack::Initialize();
  Features::AntiCheat::Initialize();
  Features::HourglassBypass::Initialize();
  Features::CurioHacks::Initialize();
  Features::EngineerHacks::Initialize();
  Features::HunterHacks::Initialize();
  Features::Bonfire::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::Bonfire::Uninitialize();
  Features::EngineerHacks::Uninitialize();
  Features::CurioHacks::Uninitialize();
  Features::HourglassBypass::Uninitialize();
  Features::GodMode::Uninitialize();
  Features::AuraKill::Uninitialize();
  Features::ExpMultiplier::Uninitialize();
  Features::InfiniteItems::Uninitialize();
  Features::Enhancement::Uninitialize();
  Features::InfiniteCurrency::Uninitialize();
  Features::FreeStore::Uninitialize();
  Features::SpeedHack::Uninitialize();
  Features::AntiCheat::Uninitialize();

  // Disable/uninitialize MinHook here
  MH_DisableHook(MH_ALL_HOOKS);
  MH_Uninitialize();
}
