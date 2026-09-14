#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

namespace Menu
{
  struct ConfigData
  {
    bool  bMenuOpen                  = true;
    bool  bGodMode                   = false;
    bool  bGodMode_Nullify           = true;
    bool  bGodMode_Damage            = true;
    bool  bGodMode_Speed             = false;
    float fGodModeDamage             = 1000.0f;
    float fGodModeSpeedMultiplier    = 5.0f;
    bool  bFastMobSpawn              = false;
    bool  bFastGathering             = false;
    bool  bAuraKill                  = false;
    bool  bExpMultiplier             = false;
    float fExpMultiplierValue        = 100.0f;
    bool  bInfiniteItems             = false;
    bool  bEnhanceItem100            = false;
    bool  bInfiniteCurrency          = false;
    bool  bFreeStore                 = false;
    bool  bSpeedHack                 = false;
    float fSpeedMultiplier           = 2.0f;
    bool  bHourglassBypass           = false;
    bool  bHourglassFreeTalents      = false;
    bool  bHourglassUnlockAllTalents = false;
    bool  bHourglassMassiveRemort    = false;
    bool  bInfiniteSand              = false;
    bool  bFastTimeLine              = false;
    float fTimeLineMultiplier        = 10.0f;

    void LoadConfig();
    void SaveConfig();
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
