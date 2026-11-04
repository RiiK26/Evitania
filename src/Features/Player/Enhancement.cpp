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

#include "Enhancement.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace Enhancement
  {
    float (*Orig_EnhanceStationService_GetSuccessChance)(void* __this, void* itemDetail, void* method_info);

    float Hook_EnhanceStationService_GetSuccessChance(void* __this, void* itemDetail, void* method_info)
    {
      if (Menu::Config.bEnhanceItem100)
        return 1.0f;  // 100%
      return Orig_EnhanceStationService_GetSuccessChance(__this, itemDetail, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "Services.EnhanceStationService", "GetSuccessChance", 1, Hook_EnhanceStationService_GetSuccessChance,
        Orig_EnhanceStationService_GetSuccessChance
      );
    }

    void Uninitialize() { }
  }  // namespace Enhancement
}  // namespace Features
