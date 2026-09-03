#include "FastMobSpawn.hpp"
#include "../../Modules/Hooks/Hooks.hpp"
#include "../../Modules/Hooks/Signatures.hpp"
#include "../../Modules/Menu/Menu.hpp"

namespace Features
{
  namespace FastMobSpawn
  {
    float (*Orig_SpawnPortal_CurrentSpawnInterval)(void* __this, void* method_info);

    float Hook_SpawnPortal_CurrentSpawnInterval(void* __this, void* method_info)
    {
      if (Menu::Config.bFastMobSpawn) {
        return 0.1f;  // Fast respawn
      }
      return Orig_SpawnPortal_CurrentSpawnInterval(__this, method_info);
    }

    void Initialize()
    {
      HOOK_SIGNATURE(
        "SpawnPortal::CurrentSpawnInterval", Signatures::SpawnPortal_CurrentSpawnInterval,
        Hook_SpawnPortal_CurrentSpawnInterval, Orig_SpawnPortal_CurrentSpawnInterval
      );
    }

    void Uninitialize() { }
  }  // namespace FastMobSpawn
}  // namespace Features
