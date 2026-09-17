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
      if (Menu::Config.bAlwaysLegendaryCurio)
        return 4;  // CurioRarity.Legendary = 4
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
