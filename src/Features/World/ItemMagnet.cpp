#include "ItemMagnet.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include "../../Utils/Offsets.h"
#include <cstdint>

namespace Features
{
  namespace ItemMagnet
  {
    void (*Orig_LootItem_BeginLife)(void* __this, Unity::Vector2 force, void* method_info);

    void Hook_LootItem_BeginLife(void* __this, Unity::Vector2 force, void* method_info)
    {
      Orig_LootItem_BeginLife(__this, force, method_info);

      if (Menu::Config.bMagnet) {
        void* target = (void*) ((uintptr_t) IL2CPP::Globals.m_GameAssembly + 0x7A64D0);  // LootItem_Collect
        if (target) {
          reinterpret_cast<int(UNITY_CALLING_CONVENTION)(void*, bool)>(target)(__this, true);
        }
      }
    }

    void Initialize()
    {
      HOOK_OFFSET("LootItem::BeginLife", Offsets::LootItem_BeginLife, Hook_LootItem_BeginLife, Orig_LootItem_BeginLife);
    }

    void Uninitialize() { }
  }  // namespace ItemMagnet
}  // namespace Features
