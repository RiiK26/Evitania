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

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif

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
    bool  bForceCurioRarity       = false;
    int   iCurioRarity            = 4;
    bool  bFreeCurioUpgrades      = false;
    bool  bHourglassBypass        = false;
    bool  bFreeSandUpgrade        = false;
    bool  bFreeUpgradeGenerator   = false;
    bool  bFreeTimelineNodes      = false;
    bool  bFreeAwakeningGenerator = false;
    bool  bFreeRestorationNodes   = false;
    bool  bFreeEngineerUpgrades   = false;
    bool  bFreeHunterUpgrades     = false;
    bool  bAlwaysLitBonfire       = false;
    bool  bFreeAshUpgrade         = false;
    bool  bFreeSacrificeCost      = false;
    bool  bStageHopperLoop        = false;
    float fStageHopperDelay       = 5.0f;
    char  szStageHopperTarget[32] = "1.0";
    char  szStageHopperStart[32]  = "1.0";

    void LoadConfig();
    void SaveConfig();
  };

  extern ConfigData Config;

  void Initialize();
  void Uninitialize();
}  // namespace Menu
