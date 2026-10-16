#include "Menu.hpp"
#include "MenuInternal.hpp"
#include "imgui.h"
#include "../../Features/Player/SpeedHack.hpp"
#include "../../Features/Player/StageHopper.hpp"

namespace Menu
{
  void DrawMenuUI()
  {
    Features::StageHopper::Update();

    if (Config.bMenuOpen) {
      ImGui::SetNextWindowSize(ImVec2(650, 650), ImGuiCond_FirstUseEver);
      ImGui::Begin(
        "Evitania Online v" PROJECT_VERSION, nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
      );

      if (ImGui::BeginTabBar("CheatTabs", ImGuiTabBarFlags_FittingPolicyScroll)) {
        // Combat Tab menu
        if (ImGui::BeginTabItem("Combat")) {
          ImGui::BeginChild("CombatChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("God Mode", &Config.bGodMode);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Master God Mode toggle");
          }

          if (ImGui::TreeNode("God Mode Settings")) {
            ImGui::Checkbox("Infinite HP", &Config.bGodMode_Nullify);
            ImGui::Checkbox("High Damage", &Config.bGodMode_Damage);
            ImGui::TextWrapped("Damage Value");
            ImGui::InputFloat("##GodModeDamage", &Config.fGodModeDamage);
            if (Config.fGodModeDamage < 0.0f)
              Config.fGodModeDamage = 0.0f;

            ImGui::Checkbox("Movement Speed", &Config.bGodMode_Speed);
            ImGui::TextWrapped("Speed Multiplier");
            ImGui::SliderFloat("##GodModeSpeed", &Config.fGodModeSpeedMultiplier, 1.0f, 10.0f);
            ImGui::TreePop();
          }
          ImGui::Checkbox("Aura Kill", &Config.bAuraKill);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("The Damage taken from GodMode high damage value");
          }

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Player Tab menu
        if (ImGui::BeginTabItem("Player")) {
          ImGui::BeginChild("PlayerChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Exp Multiplier", &Config.bExpMultiplier);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("EXP multiplier have progress account detection, use at ur own risk");
          }
          ImGui::TextWrapped("Multiplier Value");
          ImGui::InputFloat("##ExpMultiplier", &Config.fExpMultiplierValue);
          if (Config.fExpMultiplierValue < 1.0f)
            Config.fExpMultiplierValue = 1.0f;

          if (ImGui::Checkbox("Speed Hack", &Config.bSpeedHack)) {
            Features::SpeedHack::ApplySpeedHack();
          }
          ImGui::TextWrapped("Speed Value");
          if (ImGui::SliderFloat("##SpeedHack", &Config.fSpeedMultiplier, 1.0f, 10.0f)) {
            Features::SpeedHack::ApplySpeedHack();
          }

          ImGui::Checkbox("100% Success enhance Item", &Config.bEnhanceItem100);

          ImGui::Separator();
          ImGui::TextColored(ImVec4(0.3f, 0.85f, 1.0f, 1.0f), "Stage Hopper");
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Automated stage hopper to teleport and loop between current and target stage");
          }

          std::string curStage = Features::StageHopper::GetCurrentStage();
          ImGui::Text("Current Position:");
          ImGui::SameLine();
          ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "%s", curStage.c_str());

          ImGui::Text("Target Stage:");
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("use current position for a hint where do you want to go");
          }
          ImGui::InputText("##HopperTargetInput", Config.szStageHopperTarget, sizeof(Config.szStageHopperTarget));

          bool bHasTarget   = Config.szStageHopperTarget[0] != '\0';
          bool bTargetValid = bHasTarget && Features::StageHopper::IsValidStage(Config.szStageHopperTarget);

          if (bHasTarget) {
            if (bTargetValid) {
              ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.4f, 1.0f), "[Stage valid]");
            }
            else {
              ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "[Invalid stage: does not exist]");
            }
          }

          if (ImGui::Button("Hop once to the target stage", ImVec2(-1, 26))) {
            if (bTargetValid) {
              Features::StageHopper::HopOnce();
            }
          }

          ImGui::Checkbox("Enable Loop (Current <-> Target)", &Config.bStageHopperLoop);
          if (Config.bStageHopperLoop) {
            ImGui::SameLine();
            if (bTargetValid) {
              ImGui::TextColored(
                ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "[ACTIVE: %s <-> %s]",
                Config.szStageHopperStart[0] ? Config.szStageHopperStart : curStage.c_str(), Config.szStageHopperTarget
              );
            }
            else {
              ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "[PAUSED: Target stage invalid]");
            }
          }

          ImGui::SliderFloat("Loop Delay (s)##HopperDelay", &Config.fStageHopperDelay, 1.0f, 60.0f, "%.1f s");

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Hourglass Tab menu
        if (ImGui::BeginTabItem("Hourglass")) {
          ImGui::BeginChild("HourglassChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Use Timeskip hourglass anywhere", &Config.bHourglassBypass);
          ImGui::Checkbox("Free Timeline upgrade tree", &Config.bFreeTimelineNodes);
          ImGui::Checkbox("Free Hourglass upgrades", &Config.bFreeHourglassUpgrades);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Free generators, upgrades, hard upgrades, talents, and shop items");
          }

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Economy Tab menu
        if (ImGui::BeginTabItem("Economy")) {
          ImGui::BeginChild("EconomyChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Infinite items on inventory", &Config.bInfiniteItems);

          ImGui::Checkbox("Infinite currency", &Config.bInfiniteCurrency);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Diamonds, Gold, Sands, etc");
          }

          ImGui::Checkbox("Free Store", &Config.bFreeStore);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Bypass InAppPurchases");
          }

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Curio Tab menu
        if (ImGui::BeginTabItem("Curio")) {
          ImGui::BeginChild("CurioChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Force Curio Rarity", &Menu::Config.bForceCurioRarity);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Forces every gacha pull to drop a specific rarity Curio");
          }

          const char* rarities[] = {"Common", "Uncommon", "Rare", "Epic", "Legendary"};
          ImGui::Combo("Rarity", &Menu::Config.iCurioRarity, rarities, IM_ARRAYSIZE(rarities));

          ImGui::Checkbox("Free Curio Upgrades", &Menu::Config.bFreeCurioUpgrades);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Upgrading curio levels costs 0");

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Engineer Tab menu
        if (ImGui::BeginTabItem("Engineer")) {
          ImGui::BeginChild("EngineerChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Free Engineer Upgrades", &Menu::Config.bFreeEngineerUpgrades);

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        // Hunter Tab menu
        if (ImGui::BeginTabItem("Hunter")) {
          ImGui::BeginChild("HunterChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Free Hunter Upgrades", &Menu::Config.bFreeHunterUpgrades);

          ImGui::EndChild();
          ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Bonfire")) {
          ImGui::BeginChild("BonfireChild", ImVec2(0, -50), 0);

          ImGui::Checkbox("Always Lit Bonfire", &Menu::Config.bAlwaysLitBonfire);
          ImGui::SameLine();
          ImGui::TextDisabled("(?)");
          if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Still Experimental, need to verified if the fire not extinguished");

          ImGui::Checkbox("Free Ash Upgrade", &Menu::Config.bFreeAshUpgrade);
          ImGui::Checkbox("Free Sacrifice Cost", &Menu::Config.bFreeSacrificeCost);

          ImGui::EndChild();
          ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
      }

      // Save Config button
      ImGui::Separator();
      if (ImGui::Button("Save Config", ImVec2(-1, 0))) {
        Config.SaveConfig();
      }
      ImGui::Separator();
      ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "%s", "[INSERT] show/hide menu");

      ImGui::End();
    }
  }

}  // namespace Menu
