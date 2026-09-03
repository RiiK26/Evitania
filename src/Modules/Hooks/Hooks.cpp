#include "Hooks.hpp"

#include "../../Features/Combat/GodMode.hpp"
#include "../../Features/Combat/AuraKill.hpp"
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/Economy/Enhancement.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"

void Hooks::Initialize()
{
  // Initialize modular features
  Features::GodMode::Initialize();
  Features::AuraKill::Initialize();
  Features::InfiniteItems::Initialize();
  Features::Enhancement::Initialize();
  Features::AntiCheat::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::GodMode::Uninitialize();
  Features::AuraKill::Uninitialize();
  Features::InfiniteItems::Uninitialize();
  Features::Enhancement::Uninitialize();
  Features::AntiCheat::Uninitialize();
}
