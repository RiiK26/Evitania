#include "FreeStore.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Hooks/Offsets.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include "../../Cores/Scanner.hpp"

namespace Features
{
  namespace FreeStore
  {
    void (*Orig_SteamPurchaseService_InitiatePurchase)(void* __this, void* productId, void* method_info);
    void (*Orig_MobilePurchaseService_InitiatePurchase)(void* __this, void* productId, void* method_info);

    void BypassPurchase(void* __this, void* productId, void* method_info, const char* findLotSignature, void* origFunc)
    {
      if (Menu::Config.bFreeStore) {
        // Resolve FindLot function
        uintptr_t findLotAddr = Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, findLotSignature);
        void* (*FindLot)(void*, void*, void*) = (void* (*) (void*, void*, void*) ) findLotAddr;

        if (!FindLot)
          return;  // Fail safe

        void* lot = FindLot(__this, productId, nullptr);

        if (__this) {
          // IAPRewarder is at offset Offsets::Fields::SteamPurchaseService::_rewarder in both SteamPurchaseService and MobilePurchaseService
          void* rewarder = *(void**) ((uintptr_t) __this + Offsets::Fields::SteamPurchaseService::_rewarder);
          if (rewarder) {
            // public bool Reward(IAPLot item, bool isFirstPurchase = False, bool ignoreCurrencyCap = False)
            uintptr_t rewardAddr =
              Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signatures::IAPRewarder_Reward);
            if (!rewardAddr)
              return;

            bool (*Reward)(void*, void*, bool, bool, void*) = (bool (*)(void*, void*, bool, bool, void*))(rewardAddr);

            Reward(rewarder, lot, false, true, nullptr);
            return;  // Exit without calling the original InitiatePurchase
          }
        }
      }

      // If free store is off or something failed, call original
      if (origFunc == Orig_SteamPurchaseService_InitiatePurchase)
        Orig_SteamPurchaseService_InitiatePurchase(__this, productId, method_info);
      else
        Orig_MobilePurchaseService_InitiatePurchase(__this, productId, method_info);
    }

    void Hook_SteamPurchaseService_InitiatePurchase(void* __this, void* productId, void* method_info)
    {
      BypassPurchase(
        __this, productId, method_info, Signatures::SteamPurchaseService_FindLot,
        (void*) Orig_SteamPurchaseService_InitiatePurchase
      );
    }

    void Hook_MobilePurchaseService_InitiatePurchase(void* __this, void* productId, void* method_info)
    {
      BypassPurchase(
        __this, productId, method_info, Signatures::MobilePurchaseService_FindLot,
        (void*) Orig_MobilePurchaseService_InitiatePurchase
      );
    }

    void Initialize()
    {
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
