#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Menu
{
  struct ConfigData
  {
    bool  bMenuOpen               = true;
    bool  bGodMode                = false;
    bool  bGodMode_Nullify        = true;
    bool  bGodMode_Damage         = true;
    bool  bGodMode_Speed          = false;
    float fGodModeDamage          = 1000.0f;
    float fGodModeSpeedMultiplier = 5.0f;
    bool  bAuraKill               = false;
    bool  bExpMultiplier          = false;
    float fExpMultiplierValue     = 100.0f;
    bool  bInfiniteItems          = false;
    bool  bEnhanceItem100         = false;
    bool  bInfiniteCurrency       = false;
    bool  bFreeStore              = false;
    bool  bSpeedHack              = false;
    float fSpeedMultiplier        = 2.0f;
    bool  bAlwaysLegendaryCurio   = false;
    bool  bFreeCurioUpgrades      = false;
    bool  bHourglassBypass        = false;
    bool  bFreeEngineerUpgrades   = false;
    bool  bFreeHunterUpgrades     = false;

    // Bonfire

    bool bAlwaysLitBonfire  = false;
    bool bFreeAshUpgrade    = false;
    bool bFreeSacrificeCost = false;

    void LoadConfig();
    void SaveConfig();
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
