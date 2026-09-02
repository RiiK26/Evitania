#include "MonsterSpawner.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Menu/Menu.hpp"
#include <cstdint>

namespace Features
{
  namespace MonsterSpawner
  {
    int (*Orig_EnemySpawnerService_get_MaxSpawns)(void* __this, void* method_info);
    void (*Orig_EnemySpawnerService_Tick)(void* __this, void* method_info);

    int Hook_EnemySpawnerService_get_MaxSpawns(void* __this, void* method_info)
    {
      // Monster Spawn Count Modifier
      return 999;
    }

    void Hook_EnemySpawnerService_Tick(void* __this, void* method_info)
    {
      if (Menu::Config.bMonsterInstantRespawn)
        *(float*) ((uintptr_t) __this + 0x3C) = 0.0f;
      Orig_EnemySpawnerService_Tick(__this, method_info);
    }

    void Initialize()
    {
      HOOK_METHOD(
        "Services.Difficulity.EnemySpawnerService", "get_MaxSpawns", 0, Hook_EnemySpawnerService_get_MaxSpawns,
        Orig_EnemySpawnerService_get_MaxSpawns
      );
      HOOK_METHOD(
        "Services.Difficulity.EnemySpawnerService", "Tick", 0, Hook_EnemySpawnerService_Tick,
        Orig_EnemySpawnerService_Tick
      );
    }

    void Uninitialize() { }
  }  // namespace MonsterSpawner
}  // namespace Features
