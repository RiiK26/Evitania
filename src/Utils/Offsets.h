#pragma once
#include <cstdint>

namespace Offsets
{
  // Services.CurrencyService
  constexpr uintptr_t CurrencyService_Set1     = 0x6BF4A0;  // Set(CurrencyType, float)
  constexpr uintptr_t CurrencyService_Set2     = 0x6BF5D0;  // Set(Currency, float)
  constexpr uintptr_t CurrencyService_Subtract = 0x6BF890;

  // LootItem
  constexpr uintptr_t LootItem_BeginLife = 0x7A62D0;

  // Player.Skills.Skill
  constexpr uintptr_t Skill_AddExperience = 0x746B40;
}  // namespace Offsets
