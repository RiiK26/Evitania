#include "FreeStore.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"

#include "../../Modules/Menu/Menu.hpp"
#include "../../Cores/Scanner.hpp"

namespace Features
{
  namespace FreeStore
  {
    void (*Orig_SteamPurchaseService_InitiatePurchase)(void* __this, void* productId, void* method_info);
    void (*Orig_MobilePurchaseService_InitiatePurchase)(void* __this, void* productId, void* method_info);

    void* (*FindLot)(void*, void*, void*)           = nullptr;
    bool (*Reward)(void*, void*, bool, bool, void*) = nullptr;

    static int rewarderOffset = -1;

    void Hook_SteamPurchaseService_InitiatePurchase(void* __this, void* productId, void* method_info)
    {
      if (Menu::Config.bFreeStore) {
        if (!FindLot)
          return;  // Fail safe

        void* lot = FindLot(__this, productId, nullptr);

        if (__this) {
          if (rewarderOffset == -1) {
            rewarderOffset =
              IL2CPP::Class::Utils::GetFieldOffset("Services.GemShopServices.SteamPurchaseService", "_rewarder");
          }
          if (rewarderOffset > 0) {
            // get the IAPRewarder
            void* rewarder = *(void**) ((uintptr_t) __this + rewarderOffset);
            if (rewarder) {
              if (!Reward)
                return;

              Reward(rewarder, lot, false, false, nullptr);
              return;  // Bypass purchase
            }
          }
        }
      }
      Orig_SteamPurchaseService_InitiatePurchase(__this, productId, method_info);
    }

    void Hook_MobilePurchaseService_InitiatePurchase(void* __this, void* productId, void* method_info)
    {
      if (Menu::Config.bFreeStore) {
        if (!FindLot)
          return;

        void* lot = FindLot(__this, productId, nullptr);

        if (__this) {
          if (rewarderOffset == -1) {
            rewarderOffset =
              IL2CPP::Class::Utils::GetFieldOffset("Services.GemShopServices.SteamPurchaseService", "_rewarder");
          }
          if (rewarderOffset > 0) {
            void* rewarder = *(void**) ((uintptr_t) __this + rewarderOffset);
            if (rewarder) {
              if (!Reward)
                return;

              Reward(rewarder, lot, false, false, nullptr);
              return;
            }
          }
        }
      }
      Orig_MobilePurchaseService_InitiatePurchase(__this, productId, method_info);
    }

    void Initialize()
    {
      // Resolve helper functions dynamically
      uintptr_t findLotAddr =
        Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signatures::SteamPurchaseService_FindLot);
      if (findLotAddr) {
        FindLot = (void* (*) (void*, void*, void*) ) findLotAddr;
      }

      uintptr_t rewardAddr =
        Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signatures::IAPRewarder_Reward);
      if (rewardAddr) {
        Reward = (bool (*)(void*, void*, bool, bool, void*)) rewardAddr;
      }

      HOOK_SIGNATURE(
        "SteamPurchaseService::InitiatePurchase", Signatures::SteamPurchaseService_InitiatePurchase,
        Hook_SteamPurchaseService_InitiatePurchase, Orig_SteamPurchaseService_InitiatePurchase
      );
      HOOK_SIGNATURE(
        "MobilePurchaseService::InitiatePurchase", Signatures::MobilePurchaseService_InitiatePurchase,
        Hook_MobilePurchaseService_InitiatePurchase, Orig_MobilePurchaseService_InitiatePurchase
      );
    }

    void Uninitialize() { }
  }  // namespace FreeStore
}  // namespace Features
