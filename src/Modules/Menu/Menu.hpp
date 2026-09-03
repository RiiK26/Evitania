#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Menu
{
  struct ConfigData
  {
    bool  bMenuOpen       = false;
    bool  bGodMode        = false;
    float fGodModeDamage  = 1000.0f;
    bool  bAuraKill       = false;
    bool  bInfiniteItems  = false;
    bool  bEnhanceItem100 = false;

    void LoadConfig();
    void SaveConfig();
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
