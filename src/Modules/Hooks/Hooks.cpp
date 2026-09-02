#include "Hooks.hpp"
#include "./Il2CppResolver/IL2CPP_Resolver.hpp"
#include "MinHook.h"
#include "../Menu/Menu.hpp"
#include <iostream>

extern void Log(const char* msg);

#include "Hooks.hpp"
#include "MinHook.h"
#include "../Menu/Menu.hpp"
#include <iostream>

#include "../../Features/Combat/GodMode.hpp"
#include "../../Features/Combat/AuraKill.hpp"
#include "../../Features/Progression/ExpMultiplier.hpp"
#include "../../Features/World/MonsterSpawner.hpp"
#include "../../Features/World/ItemMagnet.hpp"
#include "../../Features/Economy/InfiniteCurrencies.hpp"
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/Crafting/Enhancement.hpp"
#include "../../Features/Crafting/FreeCrafting.hpp"
#include "../../Features/Crafting/FastProduction.hpp"
#include "../../Features/Vendor/DiscountedVendor.hpp"
#include "../../Features/Vendor/IncreasedVendorStock.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"

void Hooks::Initialize()
{

  // Initialize modular features
  Features::GodMode::Initialize();
  Features::AuraKill::Initialize();
  Features::ExpMultiplier::Initialize();
  Features::MonsterSpawner::Initialize();
  Features::ItemMagnet::Initialize();
  Features::InfiniteCurrencies::Initialize();
  Features::InfiniteItems::Initialize();
  Features::Enhancement::Initialize();
  Features::FreeCrafting::Initialize();
  Features::FastProduction::Initialize();
  Features::DiscountedVendor::Initialize();
  Features::IncreasedVendorStock::Initialize();
  Features::AntiCheat::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::GodMode::Uninitialize();
  Features::AuraKill::Uninitialize();
  Features::ExpMultiplier::Uninitialize();
  Features::MonsterSpawner::Uninitialize();
  Features::ItemMagnet::Uninitialize();
  Features::InfiniteCurrencies::Uninitialize();
  Features::InfiniteItems::Uninitialize();
  Features::Enhancement::Uninitialize();
  Features::FreeCrafting::Uninitialize();
  Features::FastProduction::Uninitialize();
  Features::DiscountedVendor::Uninitialize();
  Features::IncreasedVendorStock::Uninitialize();
  Features::AntiCheat::Uninitialize();
}
