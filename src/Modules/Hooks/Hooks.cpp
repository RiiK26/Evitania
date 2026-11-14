/*
 * MIT License
 *
 * Copyright © 2026 - Present RiiK
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Hooks.hpp"
#include "Signatures.hpp"
#include "../Menu/Menu.hpp"

#include "../../Features/Combat/GodMode.hpp"
#include "../../Features/Combat/AuraKill.hpp"
#include "../../Features/Player/ExpMultiplier.hpp"
#include "../../Features/Economy/InfiniteItems.hpp"
#include "../../Features/Player/Enhancement.hpp"
#include "../../Features/Economy/InfiniteCurrency.hpp"
#include "../../Features/Economy/FreeStore.hpp"
#include "../../Features/Player/SpeedHack.hpp"
#include "../../Features/AntiCheat/AntiCheat.hpp"
#include "../../Features/Hourglass/HourglassBypass.hpp"
#include "../../Features/Hourglass/HourglassUpgrades.hpp"
#include "../../Features/Curio/CurioHacks.hpp"
#include "../../Features/Engineer/EngineerHacks.hpp"
#include "../../Features/Hunter/HunterHacks.hpp"
#include "../../Features/Bonfire/Bonfire.hpp"
#include "../../Features/Player/StageHopper.hpp"

#include "MinHook.h"

bool Hooks::bHooksFailed = false;

namespace
{
  bool (*Orig_ShopSpendingService_CanSpend)(void* __this, void* price, void* source, void* method_info);
  bool Hook_ShopSpendingService_CanSpend(void* __this, void* price, void* source, void* method_info)
  {
    if (Features::Bonfire::IsFreeSpending() || Menu::Config.bFreeHunterUpgrades) {
      return true;
    }
    if (Features::HourglassUpgrades::IsPurchasing()) {
      return true;
    }
    return Orig_ShopSpendingService_CanSpend(__this, price, source, method_info);
  }

  bool (*Orig_ShopSpendingService_Spend)(void* __this, void* price, void* source, void* method_info);
  bool Hook_ShopSpendingService_Spend(void* __this, void* price, void* source, void* method_info)
  {
    if (Features::Bonfire::IsFreeSpending() || Features::HunterHacks::IsInHunterBuy()) {
      return true;
    }
    if (Features::HourglassUpgrades::IsPurchasing()) {
      return true;
    }
    return Orig_ShopSpendingService_Spend(__this, price, source, method_info);
  }
}  // namespace

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
  Features::HourglassUpgrades::Initialize();
  Features::CurioHacks::Initialize();
  Features::EngineerHacks::Initialize();
  Features::HunterHacks::Initialize();
  Features::Bonfire::Initialize();

  HOOK_SIGNATURE(
    "ShopSpendingService::CanSpend", Signatures::ShopSpendingService_CanSpend, Hook_ShopSpendingService_CanSpend,
    Orig_ShopSpendingService_CanSpend
  );
  HOOK_SIGNATURE(
    "ShopSpendingService::Spend", Signatures::ShopSpendingService_Spend, Hook_ShopSpendingService_Spend,
    Orig_ShopSpendingService_Spend
  );
  Features::StageHopper::Initialize();
}

void Hooks::Uninitialize()
{
  // MinHook handles uninitialization
  Features::StageHopper::Uninitialize();
  Features::Bonfire::Uninitialize();
  Features::EngineerHacks::Uninitialize();
  Features::CurioHacks::Uninitialize();
  Features::HourglassUpgrades::Uninitialize();
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
