#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Menu
{
  struct ConfigData
  {
    bool bMenuOpen = true;

    // Features
    bool bGodMode               = false;
    bool bAuraKill              = false;
    bool bExpMultiplier         = false;
    bool bMonsterInstantRespawn = false;
    bool bEnhanceItem100        = false;
    bool bMagnet                = false;

    bool bFreeCrafting         = false;
    bool bInfiniteGold         = false;
    bool bInfiniteItems        = false;
    bool bDiscountedVendor     = false;
    bool bIncreasedVendorStock = false;
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
