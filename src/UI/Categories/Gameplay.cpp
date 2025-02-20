#include "UI/Categories/Gameplay.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

    void CategoryGameplay::GameModeOptions(const char* a_title, GameplayActorSettings* a_Settings) {

        const char* T0 = "Select the game mode\n\n"
            "Basic:\n"
            "- Grow: Slowly grow to your size limit.\n"
            "- Shrink: Slowly shrink back to your natural scale.\n"
            "- Combat Growth: Grow during combat and shrink outside of combat back to your natural scale.\n"
            "- Slow Combat Growth: Slowly grow during combat and retain any size gained.\n\n"
            "Curses:\n"
            "- Curse of Growth: You will continiously grow in size like \"Grow\" but in spurts of varying strength up until the specified scale limit which you can change below.\n"
            "- Curse of the Giantess: You will rapidly grow to the specified size if you are smaller. Spells like \"Restore Size\" will not shrink you below this size.\n"
            "- Curse of Diminishing: You will slowly shrink to the target scale if too large.\n"
            "- Size Locked: Combines the effects of both curses. You will grow to the specified size and slowly shrink back to it if larger.";

        const char* T1 = "Modify the amount grown each tick.";
        const char* T2 = "Modify the amount shrunk each tick.";

        const char* T3 = "Set the maximum size for the \"Curse of Growth\" game mode";

        const char* T4 = "Multiply the size-gain rate by 25%% of your current size.\n"
            "The amount gained caps out at 10x scale.";

        const char* T5 = "Set the target scale to grow/shrink to when using the following game modes:\n\n"
            "- Curse of the Giantess\n"
            "- Curse of the Diminishing\n"
            "- Size Locked";

        const char* T6 = "Change how often the curse effects should be applied.\n"
            "The value you select is offset by +/- 10%% each time.";

        // TODO Check for perk
        bool HasPerk = true;

        const char* Reason;
        if (Config::GetBalance().bBalanceMode) {
            Reason = "Balance Mode Active";
            HasPerk = false;
        }
        else {
            Reason = "Requires \"Colossal Growth\" Perk";
        }

        if (ImUtil::ConditionalHeader(a_title, Reason, HasPerk, false)) {

            ImUtil::ComboEx<GameMode>("Game Mode", a_Settings->sGameMode, T0);

            ImGui::Spacing();
            ImGui::Text("Basic Game Modes");
            ImUtil::SliderF("Growth Rate", &a_Settings->fGrowthRate, 0.001f, 0.2f, T1, "%.3fx");
            ImUtil::SliderF("Shrink Rate", &a_Settings->fShrinkRate, 0.001f, 0.2f, T2, "%.3fx");
            ImUtil::SliderF("Scale Limit", &a_Settings->fGrowthSizeLimit, 1.1f, 50.0f, T3, "%.2fx");
            ImUtil::CheckBox("Multiply Rates", &a_Settings->bMultiplyGrowthrate, T4);

            ImGui::Spacing();
            ImGui::Text("Curse Game Modes");

            ImUtil::SliderF("Target Scale", &a_Settings->fCurseTargetScale, 1.1f, 2.0f, T5, "%.2fx");
            ImUtil::SliderF("Curse Update Interval", &a_Settings->fUpdateInterval, 2.0f, 10.0f, T6, "Every %.2f Seconds");

            ImGui::Spacing();
        }
    }

    void CategoryGameplay::DrawLeft() {

        ImUtil_Unique {
            if (ImGui::CollapsingHeader("Perk Settings", ImUtil::HeaderFlags)) {

                // TODO Add Perk References
                const bool PerkCondCrush = true;
                const bool PerkCondHit = true;
                const bool PerkCondAtribCap = true;

                const char* T1 = "Upon crushing someone: Grow in size.\n"
                                 "Applies to both the player and followers.";

                const char* T2 = "On being hit: Grow in size.\n"
                                 "Applies to both the player and followers.";

                const char* T3 = "By default, the maximum amount of bonus attributes per player level is 2.\n"
                                 "You can adjust the multiplier for this here.\n";

                const std::string tGrowOnCrush = fmt::format("Grow On Crush {}", (!PerkCondCrush ? "[Missing Perk]" : ""));
                ImUtil::CheckBox(tGrowOnCrush.c_str(), &Settings.bEnableCrushGrowth, T1, !PerkCondCrush);

                const std::string tGrowOnHit = fmt::format("Grow On Hit {}", (!PerkCondHit ? "[Missing Perk]" : ""));
                ImUtil::CheckBox(tGrowOnHit.c_str(), &Settings.bEnableGrowthOnHit, T2, !PerkCondHit);

                const char* tSizeConfFMT = (!PerkCondAtribCap ? "[Missing Perk]" : "%.2fx");
                ImUtil::SliderF("Size Conversion Attrib. Cap", &Settings.fSizeConvLevelCap, 0.1f, 5.0f, T3, tSizeConfFMT, !PerkCondAtribCap);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique {
            if (ImGui::CollapsingHeader("Armor Stripping", ImUtil::HeaderFlags)) {

                const char* T1 = "Enable/disable the automatic unequipping of clothing/armor when large enough.\n"
                                 "Applies to both the player and followers.\n"
                                 "This system works based on size thresholds.\n"
                                 "When an armor piece is stripped, the size required for the next piece to be removed increases.";

                const char* T2 = "Set the scale threshold at which to start unequipping pieces of clothing/armor.";

                const char* T3 = "Set the scale threshold at which all clothing/armor is unequipped.";

                ImUtil::CheckBox("Enable Armor Strip", &Settings.bClothTearing, T1);
                ImUtil::SliderF("Starting Threshold", &Settings.fClothRipStart, 1.1f, 2.5f, T2, "%.2fx");
                ImUtil::SliderF("Strip All Threshold", &Settings.fClothRipThreshold, Settings.fClothRipStart + 0.1f, 3.5f, T3, "%.2fx",false, true);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            if (ImGui::CollapsingHeader("Size Effects", ImGuiTreeNodeFlags_None)) {

                const char* T1 = "When large enough, footsteps or size-related actions will launch physics-enabled items.";

                const char* T2 = "Apply the physics hit impulse to objects outside of the current cell.\n"
                                 "Beware: This is computationally very heavy. Hence the existence of this toggle.\n"
                                 "If you experience FPS drops, try disabling this.";

                const char* T3 = "Enable effects like dust and camera shake/rumble.";

                ImUtil::CheckBox("Launch Objects", &Settings.bLaunchObjects, T1);
                ImGui::SameLine();
                ImUtil::CheckBox("Enable In All Cells", &Settings.bLaunchAllCells, T2, !Settings.bLaunchObjects);
                ImUtil::CheckBox("Player Dust/Rumble Effects", &Settings.bPlayerAnimEffects, T3);
                ImUtil::CheckBox("Follower Dust/Rumble Effects", &Settings.bNPCAnimEffects, T3);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            if (ImGui::CollapsingHeader("Animations", ImGuiTreeNodeFlags_None)) {

                const char* T1 = "When enabled:\n"
                                 "Replaces the light stomp animations made by SonderBain with different\n"
                                 "versions made by NickNack.";

                const char* T2 = "This mod introduces new subtle transition animations when entering/exiting sneak or crawl states.\n"
                                 "This toggle disables/enables them.";

                ImUtil::CheckBox("Alternative Stomp Player", &Settings.bStompAlternative, T1);
                ImUtil::CheckBox("Alternative Stomp NPCs", &Settings.bStomAlternativeOther, T2);
                ImGui::Spacing();
                ImUtil::CheckBox("Sneak Transitions Player", &Settings.bSneakTransitions, T2);
                ImUtil::CheckBox("Sneak Transitions NPCs", &Settings.bSneakTransitionsOther, T2);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            if (ImGui::CollapsingHeader("Vore Settings", ImGuiTreeNodeFlags_None)) {

                const char* T1 = "Modify the amount of growth gained after vore.";
                const char* T2 = "Enable Skyrim's free camera when doing any vore actions.";
                const char* T3 = "Increase vanilla character weight after vore.";
                const char* T4 = "Allow voring spiders.";
                const char* T5 = "Allow voring undead actors (like draugr).";

                ImUtil::SliderF("Vore Gain Mult", &Settings.fVoreGainMult, 0.1f, 3.0f, T1, "%.1fx");
                ImUtil::CheckBox("Enable FreeCam During Vore", &Settings.bVoreFreecam, T2);
                ImUtil::CheckBox("Increase Character Weight After Vore", &Settings.bVoreWeightGain, T3);
                ImUtil::CheckBox("Allow Spiders", &Settings.bAllowSpiders, T4);
                ImUtil::CheckBox("Allow Undead", &Settings.bAllowUndead, T5);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            if (ImGui::CollapsingHeader("Hug Settings", ImGuiTreeNodeFlags_None)) {

                const char* T1 = "Toggle whether non lethal hug actions\n"
                                 "like Hug-Heal or Hug-Shrink should start combat.";

                ImUtil::CheckBox("Non Lethal Hugs Are Hostile", &Settings.bNonLethalHugsHostile, T1);

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{
            if (ImGui::CollapsingHeader("Cleavage Offsets", ImGuiTreeNodeFlags_None)) {

                const char* T1 = "Modify the placement of actors during cleavage actions.";

                ImUtil::SliderF("Forward/Back", &Settings.f2CleavageOffset.at(0), -10.0f, 10.0f, T1, "%.2f");
                ImUtil::SliderF("Up/Down", &Settings.f2CleavageOffset.at(1), -10.0f, 10.0f, T1, "%.2f");

                ImGui::Spacing();
            }
        }
    }

    void CategoryGameplay::DrawRight() {

        ImUtil_Unique{
            //TODO Check for perk
            bool HasPerk = true;
            const char* Reason;
            if (Config::GetBalance().bBalanceMode) {
                Reason = "Balance Mode Active";
                HasPerk = false;
            }
            else {
                Reason = "Requires \"Pleasurable Growth\" Perk";
            }

            if (ImUtil::ConditionalHeader("Random Growth", Reason, HasPerk)) {

                const char* T1 = "Change how often the random growth should trigger.\n"
                                 "Lower values = More often.\n"
                                 "Higher values = Less often.\n"
                                 "Set to 0.0 to disable it entirely.";

                const char* Fmt1 = Settings.GamemodePlayer.fRandomGrowthDelay != 0.0f ? "%.2fx" : "Disabled";
                const char* Fmt2 = Settings.GamemodeFollower.fRandomGrowthDelay != 0.0f ? "%.2fx" : "Disabled";

                ImUtil::SliderF("Growth Delay Player", &Settings.GamemodePlayer.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt1);
                ImUtil::SliderF("Growth Delay Followers", &Settings.GamemodeFollower.fRandomGrowthDelay, 0.00f, 4.0f, T1, Fmt2);

                ImGui::Spacing();
            }
        }

        // ImUtil_Unique {
        //     //TODO Check for perk
        //     const bool temp = true;
        //     if (ImUtil::ConditionalHeader("Size Difference Thresholds", "Requires \"Colossal Growth\" Perk", temp,false)) {

        //         const char* T0 = "Size difference required between the GTS and a target to start a vore action.";
        //         const char* T1 = "Size difference required between the GTS and a target to start a butt crush action.";
        //         const char* T2 = "Size difference required between the GTS and a target to start a grab action.";
        //         const char* T3 = "Size difference required between the GTS and a target to start a thigh sandwich action.";
        //         const char* T4 = "Size difference required between the GTS and a target for stomps to start doing significant damage.";
        //         const char* T5 = "Size difference required between the GTS and a target to be able to keep perfoming hugs on them.";
        //         const char* T6 = "Size difference required between the GTS AI and a target to start a thigh crush action.";

        //         ImGui::Text("Applies to both Player/Followers");
        //         ImUtil::SliderF("Start Vore", &Settings.ActionSettings.fStartVoreScale, 4.0f, 10.0f, T0, "%.2fx");
        //         ImUtil::SliderF("Start Butt Crush", &Settings.ActionSettings.fStartButtCrushScale, 1.5f, 5.0f, T1, "%.2fx");
        //         ImUtil::SliderF("Start Grab", &Settings.ActionSettings.fStartGrabScale, 5.5f, 10.0f, T2, "%.2fx");
        //         ImUtil::SliderF("Start Thigh Sandwich", &Settings.ActionSettings.fStartThighSandwichScale, 3.0f, 10.0f, T3, "%.2fx");
        //         ImUtil::SliderF("Start Stomp", &Settings.ActionSettings.fStartStompScale, 5.0f, 4.0f, T4, "%.2fx");
        //         ImUtil::SliderF("Hugs Max Size Difference", &Settings.ActionSettings.fHugDropScale, 0.7f, 1.1f, T5, "%.2fx");

        //         ImGui::Text("Applies to followers only");
        //         ImUtil::SliderF("Start Thigh Crush", &Settings.ActionSettings.fAIStartThighCrushScale, 3.0f, 10.0f, T6, "%.2fx");

        //         ImGui::Spacing();
        //     }
        // }

    	//----------- Game Modes
        ImUtil_Unique{
            GameModeOptions("Gamemode Player", &Settings.GamemodePlayer);
        }

        ImUtil_Unique{
            GameModeOptions("Gamemode Followers", &Settings.GamemodeFollower);
        }
    }
}