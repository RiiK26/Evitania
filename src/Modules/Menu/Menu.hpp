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
    bool  bGodMode        = false;
    float fGodModeDamage  = 999999999.0f;
    bool  bAuraKill       = false;
    bool  bInfiniteItems  = false;
    bool  bEnhanceItem100 = false;
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
