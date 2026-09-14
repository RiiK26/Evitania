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
#include "../../Features/Combat/FastMobSpawn.hpp"
#include "../../Features/Economy/FastGathering.hpp"
#include "../../Features/Economy/HourglassBypass.hpp"
#include "../../Features/Economy/HourglassTalents.hpp"
#include "../../Features/Economy/InfiniteSand.hpp"
#include "../../Features/Economy/FastTimeLine.hpp"

void Hooks::Initialize()
{
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
  Features::FastMobSpawn::Initialize();
  Features::FastGathering::Initialize();
  Features::HourglassBypass::Initialize();
  Features::HourglassTalents::Initialize();
  Features::InfiniteSand::Initialize();
  Features::FastTimeLine::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::HourglassBypass::Uninitialize();
  Features::HourglassTalents::Uninitialize();
  Features::InfiniteSand::Uninitialize();
  Features::FastTimeLine::Uninitialize();
  Features::GodMode::Uninitialize();
  Features::AuraKill::Uninitialize();
  Features::ExpMultiplier::Uninitialize();
  Features::InfiniteItems::Uninitialize();
  Features::Enhancement::Uninitialize();
  Features::InfiniteCurrency::Uninitialize();
  Features::FreeStore::Uninitialize();
  Features::SpeedHack::Uninitialize();
  Features::AntiCheat::Uninitialize();
}
