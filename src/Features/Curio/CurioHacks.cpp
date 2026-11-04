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

#include "CurioHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace CurioHacks
  {
    int (*Orig_CurioGachaService_RollRarity)(void* __this, void* rng, void* method_info);
    int Hook_CurioGachaService_RollRarity(void* __this, void* rng, void* method_info)
    {
      if (Menu::Config.bForceCurioRarity)
        return Menu::Config.iCurioRarity;  // 0=Common, 1=Uncommon, 2=Rare, 3=Epic, 4=Legend
      return Orig_CurioGachaService_RollRarity(__this, rng, method_info);
    }

    float (*Orig_CurioPowerService_GetLevelUpCost)(void* __this, int rarity, int currentLevel, void* method_info);
    float Hook_CurioPowerService_GetLevelUpCost(void* __this, int rarity, int currentLevel, void* method_info)
    {
      if (Menu::Config.bFreeCurioUpgrades)
        return 0.0f;
      return Orig_CurioPowerService_GetLevelUpCost(__this, rarity, currentLevel, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "CurioGachaService::RollRarity", Signatures::CurioGachaService_RollRarity, Hook_CurioGachaService_RollRarity,
        Orig_CurioGachaService_RollRarity
      );

      HOOK_SIGNATURE(
        "CurioPowerService::GetLevelUpCost", Signatures::CurioPowerService_GetLevelUpCost,
        Hook_CurioPowerService_GetLevelUpCost, Orig_CurioPowerService_GetLevelUpCost
      );
    }

    void Uninitialize() { }
  }  // namespace CurioHacks
}  // namespace Features
