#include "Hooks.hpp"

#include "../../Features/Combat/GodMode.hpp"
#include "../../Features/Combat/AuraKill.hpp"
#include "../../Features/Combat/ExpMultiplier.hpp"
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/Economy/Enhancement.hpp"
#include "../../Features/Economy/InfiniteCurrency.hpp"
#include "../../Features/Economy/FreeStore.hpp"
#include "../../Features/Combat/SpeedHack.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"
#include "../../Features/Economy/HourglassBypass.hpp"

#include "MinHook.h"

bool Hooks::bHooksFailed = false;

void Hooks::Initialize()
{
  int mhStatus = MH_Initialize();
  if (mhStatus != MH_OK && mhStatus != MH_ERROR_ALREADY_INITIALIZED) {
    Menu::Logger::Log("[Hooks] MH_Initialize failed: %d\n", mhStatus);
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


  if (!Hooks::bHooksFailed) {
    Menu::Logger::Log("[Hooks] All features initialized successfully!\n");
  }
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
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
