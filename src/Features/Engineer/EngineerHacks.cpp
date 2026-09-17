#include "EngineerHacks.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace EngineerHacks
  {
    bool (*Orig_EngineerService_TryGetUpgradeCost)(
      void* __this, int slotIndex, void* upgradeId, void* out_cost, void* method_info
    );
    bool Hook_EngineerService_TryGetUpgradeCost(
      void* __this, int slotIndex, void* upgradeId, void* out_cost, void* method_info
    )
    {
      bool result = Orig_EngineerService_TryGetUpgradeCost(__this, slotIndex, upgradeId, out_cost, method_info);
      if (Menu::Config.bFreeEngineerUpgrades && result && out_cost) {
        // UpgradeCost struct:
        // 0x0: TradableBase Item
        // 0x8: long Amount
        *(long long*) ((uintptr_t) out_cost + 0x8) = 0;
      }
      return result;
    }

    void* (*Orig_EngineerUpgradeConfig_GetPrice)(void* __this, int nextTier, void* method_info);
    void* Hook_EngineerUpgradeConfig_GetPrice(void* __this, int nextTier, void* method_info)
    {
      void* dict = Orig_EngineerUpgradeConfig_GetPrice(__this, nextTier, method_info);
      if (Menu::Config.bFreeEngineerUpgrades && dict) {
        // Clear the dictionary by setting its 'count' property to 0
        // In Unity's Il2Cpp Dictionary<K, V> implementation, the count is located at offset 0x20.
        *(int*) ((uintptr_t) dict + 0x20) = 0;
      }
      return dict;
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "EngineerService::TryGetUpgradeCost", Signatures::EngineerService_TryGetUpgradeCost,
        Hook_EngineerService_TryGetUpgradeCost, Orig_EngineerService_TryGetUpgradeCost
      );

      HOOK_SIGNATURE(
        "EngineerUpgradeConfig::GetPrice", Signatures::EngineerUpgradeConfig_GetPrice,
        Hook_EngineerUpgradeConfig_GetPrice, Orig_EngineerUpgradeConfig_GetPrice
      );
    }

    void Uninitialize() { }
  }  // namespace EngineerHacks
}  // namespace Features
