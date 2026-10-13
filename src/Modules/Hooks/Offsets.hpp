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

  }  // namespace Il2Cpp

  // Backward-compatible aliases for IL2CPP generic offsets
  constexpr int entries = Il2Cpp::Dictionary::entries;
  constexpr int count   = Il2Cpp::Dictionary::count;

  // Game Logic Field Offsets
  constexpr int isPlayer          = 0x43;
  constexpr int AttackDamage      = 0x10;
  constexpr int uniqueName        = 0x20;
  constexpr int alive             = 0x3C;
  constexpr int Amount            = 0x8;
  constexpr int cost              = 0x48;
  constexpr int itemAmount        = 0x18;
  constexpr int _portal           = 0x38;
  constexpr int difficulty        = 0x30;
  constexpr int mobKillsToOpen    = 0x20;
  constexpr int enemy             = 0x28;
  constexpr int _rewarder         = 0x20;
  constexpr int _view             = 0x28;
  constexpr int networkPlayerSync = 0x88;
  constexpr int _isNet            = 0x50;
  constexpr int EntityGuid        = 0x10;
  constexpr int XpPerHour         = 0x24;
  constexpr int KillsPerHour      = 0x28;
  constexpr int GoldPerHour       = 0x2C;
  constexpr int discount          = 0x58;
}  // namespace Offsets
