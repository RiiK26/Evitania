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
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"

void Hooks::Initialize()
{

  // Initialize modular features
  Features::GodMode::Initialize();
  Features::AuraKill::Initialize();
  Features::InfiniteItems::Initialize();
  Features::AntiCheat::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::GodMode::Uninitialize();
  Features::AuraKill::Uninitialize();
  Features::InfiniteItems::Uninitialize();
  Features::AntiCheat::Uninitialize();
}
