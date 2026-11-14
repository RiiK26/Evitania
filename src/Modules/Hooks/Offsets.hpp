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

namespace Offsets
{
  // IL2CPP Internal Structures
  namespace Il2Cpp
  {
    namespace Array
    {
      constexpr int length = 0x18;
      constexpr int data   = 0x20;
    }  // namespace Array

    namespace Dictionary
    {
      constexpr int entries     = 0x18;
      constexpr int count       = 0x20;
      constexpr int entryStride = 0x18;
      constexpr int keyOffset   = 0x8;
      constexpr int valueOffset = 0x10;
    }  // namespace Dictionary

    namespace List
    {
      constexpr int items = 0x10;
      constexpr int size  = 0x18;
    }  // namespace List

    namespace Object
    {
      constexpr int cachedPtr = 0x10;
    }  // namespace Object

  }  // namespace Il2Cpp

  // Backward-compatible aliases for IL2CPP generic offsets
  constexpr int entries = Il2Cpp::Dictionary::entries;
  constexpr int count   = Il2Cpp::Dictionary::count;

  // Game Logic Field Offsets
  constexpr int isPlayer          = 0x43;
  constexpr int AttackDamage      = 0x10;
  constexpr int _currentSpawner   = 0x38;
  constexpr int uniqueName        = 0x20;
  constexpr int alive             = 0x3C;
  constexpr int _enemyCards       = 0x30;
  constexpr int Amount            = 0x8;
  constexpr int enemies           = 0x18;
  constexpr int currency          = 0x18;
  constexpr int cost              = 0x48;
  constexpr int itemAmount        = 0x18;
  constexpr int _portal           = 0x38;
  constexpr int difficulty        = 0x30;
  constexpr int mobKillsToOpen    = 0x20;
  constexpr int enemy             = 0x28;
  constexpr int _rewarder         = 0x20;
  constexpr int _view             = 0x30;
  constexpr int networkPlayerSync = 0x88;
  constexpr int _isNet            = 0x58;
  constexpr int EntityGuid        = 0x10;
  constexpr int Skill             = 0x20;
  constexpr int XpPerHour         = 0x24;
  constexpr int KillsPerHour      = 0x28;
  constexpr int GoldPerHour       = 0x2C;
  constexpr int _cardDatabase     = 0x68;
  constexpr int _combatCalculator = 0x50;
}  // namespace Offsets
