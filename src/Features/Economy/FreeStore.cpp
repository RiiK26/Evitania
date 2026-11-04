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

    typedef void* (*PfnFindLot)(void*, void*, void*);
    typedef bool (*PfnReward)(void*, void*, bool, bool, void*);

    PfnFindLot FindLot = nullptr;
    PfnReward  Reward  = nullptr;

    void Hook_SteamPurchaseService_InitiatePurchase(void* __this, void* productId, void* method_info)
    {
      if (Menu::Config.bFreeStore) {
        if (!FindLot)
          return;  // Fail safe

        void* lot = FindLot(__this, productId, nullptr);

        if (__this) {
          if (Offsets::_rewarder > 0) {
            // get the IAPRewarder
            void* rewarder = *(void**) ((uintptr_t) __this + Offsets::_rewarder);
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
          if (Offsets::_rewarder > 0) {
            void* rewarder = *(void**) ((uintptr_t) __this + Offsets::_rewarder);
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
        FindLot = (PfnFindLot) findLotAddr;
      }

      uintptr_t rewardAddr =
        Scanner::FindPattern((HMODULE) IL2CPP::Globals.m_GameAssembly, Signatures::IAPRewarder_Reward);
      if (rewardAddr) {
        Reward = (PfnReward) rewardAddr;
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
