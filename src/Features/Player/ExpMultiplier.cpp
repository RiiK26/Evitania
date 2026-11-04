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

#include "ExpMultiplier.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace ExpMultiplier
  {
    void (*Orig_Skill_AddExperience)(void* __this, float amount, void* method_info);

    void Hook_Skill_AddExperience(void* __this, float amount, void* method_info)
    {
      if (Menu::Config.bExpMultiplier) {
        amount *= Menu::Config.fExpMultiplierValue;  // Multiply experience
      }
      Orig_Skill_AddExperience(__this, amount, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "Skill::AddExperience", Signatures::Skill_AddExperience, Hook_Skill_AddExperience, Orig_Skill_AddExperience
      );
    }

    void Uninitialize() { }
  }  // namespace ExpMultiplier
}  // namespace Features
