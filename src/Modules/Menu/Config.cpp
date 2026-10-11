#include "Menu.hpp"
#include "MenuInternal.hpp"
#include <windows.h>
#include <fstream>
#include <sstream>

namespace Menu
{
  ConfigData         Config;
  static std::string g_IniPath;

  std::string GetConfigPath()
  {
    char    path[MAX_PATH];
    HMODULE hMod = NULL;
    GetModuleHandleExA(
      GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) &GetConfigPath,
      &hMod
    );
    if (hMod && GetModuleFileNameA(hMod, path, MAX_PATH)) {
      std::string fullPath(path);
      size_t      lastSlash = fullPath.find_last_of("\\/");
      if (lastSlash != std::string::npos) {
        return fullPath.substr(0, lastSlash) + "\\config.txt";
      }
    }
    return "config.txt";
  }

  static float SafeParseFloat(const std::string& value, float defaultValue)
  {
    try {
      return std::stof(value);
    } catch (...) {
      return defaultValue;
    }
  }

  void ConfigData::LoadConfig()
  {
    std::ifstream f(GetConfigPath());
    if (!f.is_open())
      return;

    std::string line;
    while (std::getline(f, line)) {
      std::istringstream is_line(line);
      std::string        key;
      if (std::getline(is_line, key, '=')) {
        std::string value;
        if (std::getline(is_line, value)) {
          if (key == "god_mode")
            bGodMode = (value == "1");
          else if (key == "god_mode_nullify")
            bGodMode_Nullify = (value == "1");
          else if (key == "god_mode_damage_toggle")
            bGodMode_Damage = (value == "1");
          else if (key == "god_mode_speed_toggle")
            bGodMode_Speed = (value == "1");
          else if (key == "god_mode_damage")
            fGodModeDamage = SafeParseFloat(value, fGodModeDamage);
          else if (key == "god_mode_speed_multiplier")
            fGodModeSpeedMultiplier = SafeParseFloat(value, fGodModeSpeedMultiplier);
          else if (key == "aura_kill")
            bAuraKill = (value == "1");
          else if (key == "exp_multiplier")
            bExpMultiplier = (value == "1");
          else if (key == "exp_multiplier_value")
            fExpMultiplierValue = SafeParseFloat(value, fExpMultiplierValue);
          else if (key == "infinite_items")
            bInfiniteItems = (value == "1");
          else if (key == "enhance_item_100")
            bEnhanceItem100 = (value == "1");
          else if (key == "infinite_currency")
            bInfiniteCurrency = (value == "1");
          else if (key == "free_store")
            bFreeStore = (value == "1");
          else if (key == "speed_hack")
            bSpeedHack = (value == "1");
          else if (key == "speed_multiplier")
            fSpeedMultiplier = SafeParseFloat(value, fSpeedMultiplier);
          else if (key == "hourglass_bypass")
            bHourglassBypass = (value == "1");
          else if (key == "free_timeline_nodes")
            bFreeTimelineNodes = (value == "1");
          else if (key == "free_restoration_nodes")
            bFreeRestorationNodes = (value == "1");
          else if (key == "free_engineer_upgrades")
            bFreeEngineerUpgrades = (value == "1");
          else if (key == "free_hunter_upgrades")
            bFreeHunterUpgrades = (value == "1");
          else if (key == "always_lit_bonfire")
            bAlwaysLitBonfire = (value == "1");
          else if (key == "free_ash_upgrade")
            bFreeAshUpgrade = (value == "1");
          else if (key == "free_sacrifice_cost")
            bFreeSacrificeCost = (value == "1");
          else if (key == "force_curio_rarity")
            bForceCurioRarity = (value == "1");
          else if (key == "curio_rarity")
            iCurioRarity = std::stoi(value);
          else if (key == "stage_hopper_loop")
            bStageHopperLoop = (value == "1");
          else if (key == "stage_hopper_delay")
            fStageHopperDelay = SafeParseFloat(value, fStageHopperDelay);
          else if (key == "stage_hopper_target") {
            snprintf(szStageHopperTarget, sizeof(szStageHopperTarget), "%s", value.c_str());
          }
          else if (key == "stage_hopper_start") {
            snprintf(szStageHopperStart, sizeof(szStageHopperStart), "%s", value.c_str());
          }
        }
      }
    }
  }

  void ConfigData::SaveConfig()
  {
    std::ofstream out(GetConfigPath());
    if (!out.is_open())
      return;

    out << "menu_open=" << (bMenuOpen ? "1" : "0") << "\n";
    out << "god_mode=" << (bGodMode ? "1" : "0") << "\n";
    out << "god_mode_nullify=" << (bGodMode_Nullify ? "1" : "0") << "\n";
    out << "god_mode_damage_toggle=" << (bGodMode_Damage ? "1" : "0") << "\n";
    out << "god_mode_speed_toggle=" << (bGodMode_Speed ? "1" : "0") << "\n";
    out << "god_mode_damage=" << fGodModeDamage << "\n";
    out << "god_mode_speed_multiplier=" << fGodModeSpeedMultiplier << "\n";
    out << "aura_kill=" << (bAuraKill ? "1" : "0") << "\n";
    out << "exp_multiplier=" << (bExpMultiplier ? "1" : "0") << "\n";
    out << "exp_multiplier_value=" << (long long) fExpMultiplierValue << "\n";
    out << "infinite_items=" << (bInfiniteItems ? "1" : "0") << "\n";
    out << "enhance_item_100=" << (bEnhanceItem100 ? "1" : "0") << "\n";
    out << "infinite_currency=" << (bInfiniteCurrency ? "1" : "0") << "\n";
    out << "free_store=" << (bFreeStore ? "1" : "0") << "\n";
    out << "speed_hack=" << (bSpeedHack ? "1" : "0") << "\n";
    out << "speed_multiplier=" << fSpeedMultiplier << "\n";
    out << "hourglass_bypass=" << (bHourglassBypass ? "1" : "0") << "\n";
    out << "free_timeline_nodes=" << (bFreeTimelineNodes ? "1" : "0") << "\n";
    out << "free_restoration_nodes=" << (bFreeRestorationNodes ? "1" : "0") << "\n";
    out << "free_engineer_upgrades=" << (bFreeEngineerUpgrades ? "1" : "0") << "\n";
    out << "free_hunter_upgrades=" << (bFreeHunterUpgrades ? "1" : "0") << "\n";
    out << "always_lit_bonfire=" << (bAlwaysLitBonfire ? "1" : "0") << "\n";
    out << "free_ash_upgrade=" << (bFreeAshUpgrade ? "1" : "0") << "\n";
    out << "free_sacrifice_cost=" << (bFreeSacrificeCost ? "1" : "0") << "\n";
    out << "force_curio_rarity=" << (bForceCurioRarity ? "1" : "0") << "\n";
    out << "curio_rarity=" << iCurioRarity << "\n";
    out << "stage_hopper_loop=" << (bStageHopperLoop ? "1" : "0") << "\n";
    out << "stage_hopper_delay=" << fStageHopperDelay << "\n";
    out << "stage_hopper_target=" << szStageHopperTarget << "\n";
    out << "stage_hopper_start=" << szStageHopperStart << "\n";
  }

}  // namespace Menu
