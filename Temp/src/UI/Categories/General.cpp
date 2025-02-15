#include "General.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

	void CategoryGeneral::DrawLeft() {


	    ImUtil_Unique {

	        const char* T0 = "Protect essential NPCs from being crushed, eaten, or affected by size-related spells/actions.";
	        const char* T1 = "Protect followers from being crushed, eaten, or affected by size-related spells/actions.";

	        if (ImGui::CollapsingHeader("Protect Actors", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Protect Essential NPCs",&Settings.bProtectEssentials, T0);
	            ImUtil::CheckBox("Protect Followers",&Settings.bProtectFollowers, T1);
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Enable or disable compatibility with the Devourment mod.\n"
	                         "If enabled, when this mod's vore actions are completed, the NPC will be delegated to the Devourment mod.";

	        const char* T1 = "Enable or disable compatibility with the Alternate Conversation Camera mod.\n"
	                         "If enabled, this mod's camera offsets during dialogue will be disabled.";

	        if (ImGui::CollapsingHeader("Compatibility", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Devourment Compatibility",&Settings.bDevourmentCompat, T0);
	            ImUtil::CheckBox("Alt Conversation Cam. Compatibility",&Settings.bConversationCamCompat, T1);
	            ImGui::Spacing();

	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Reduce the amount of gore in some sound and visual effects.";
	        const char* T1 = "Show heart particle effects during certain actions.";
	        const char* T2 = "Show or hide icons above NPCs indicating which GTS actions can be performed on them.";

	        if (ImGui::CollapsingHeader("Visuals", ImUtil::HeaderFlags)) {

	            ImUtil::CheckBox("Less Gore",&Settings.bLessGore, T0);
	            ImUtil::CheckBox("Heart Effects",&Settings.bShowHearts, T1);
	            ImUtil::CheckBox("Show Action Icons",&Settings.bShowIcons, T2);
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Male Actor Support:\n"
	                         "No support is provided for this feature.\n"
	                         "This mod was primarily designed with female NPCs in mind\n"
	                         "and always assumes the player/followers are female.\n"
	                         "Animations may not look good and could cause issues even.\n"
	                         "Use at your own risk.";

	        if (ImGui::CollapsingHeader("Experimental", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Allow Male Actors", &Settings.bEnableMales, T0);
	        }
	    }
	}

	void CategoryGeneral::DrawRight() {

	    ImUtil_Unique {

	        const char* T0 = "Enable layered sneak mechanics (applies only to third person):\n"
	                         "Layered sneak adds this mod's two extra sneak states\n"
	                         "(crawl and prone) to the sneak key (CTRL).\n\n"
	                         "Pressing the sneak button once while standing will activate normal sneak mode.\n"
	                         "Pressing it again while already sneaking will activate crawl mode.\n"
	                         "Pressing it a third time will activate prone mode.\n\n"
	                         "To go back up one state (e.g., from crawling to sneaking),\n"
	                         "hold shift while pressing the sneak button.";

	        const char* T1 = "Replace sneaking with crawling for the player only.";
	        const char* T2 = "Replace sneaking with crawling for followers.";

	        if (ImGui::CollapsingHeader("Sneaking", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Enable Layered Sneak", &Settings.bLayeredSneak, T0);

	            ImUtil::CheckBox("Enable Player Crawling", &Settings.bPlayerCrawling, T1, Settings.bLayeredSneak);
	            ImUtil::CheckBox("Enable Follower Crawling", &Settings.bFollowerCrawling, T2);
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "This toggle enables automatic size adjustment:\n"
	                         "If the player or their followers are too large to fit within a room, they will be temporarily scaled down to roughly 90%% of the room's current height.\n"
	                         "Once outside the small room, they will regrow to their previous size.";

	        const char* T1 = "Adjust the speed of all animations based on an actor's scale.";
	        const char* T2 = "Allow adjustments to the field of view during certain actions (e.g., Second Wind).";
	        const char* T3 = "Track biped skeleton bone positions during certain animated actions.";

	        if (ImGui::CollapsingHeader("Miscellaneous")) {
	            ImUtil::CheckBox("Dynamic Size Player", &Settings.bDynamicSizePlayer, T0);
	            ImUtil::CheckBox("Dynamic Size Followers", &Settings.bDynamicSizeFollowers, T0);
	            ImUtil::CheckBox("Dynamic Animation Speed", &Settings.bDynamicAnimspeed, T1);
	            ImUtil::CheckBox("Enable FOV Edits", &Settings.bEnableFOVEdits, T2);
	            ImUtil::CheckBox("Track Bones During Actions", &Settings.bTrackBonesDuringAnim, T3);
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Enable height adjustment/correction for actors wearing high heels.";
	        const char* T1 = "Disable HH height adjustments when using furniture to allow other mods to handle it.";

	        if (ImGui::CollapsingHeader("High-Heels")) {
	            ImUtil::CheckBox("Enable Height Adjustment", &Settings.bEnableHighHeels, T0);
	            ImUtil::CheckBox("Disable When Using Furniture", &Settings.bHighheelsFurniture, T1);
	        }
	    }


	    ImUtil_Unique {

	        const char* T0 = "Toggle whether actions like vore, shrink to death, or crushing\n"
	                         "should spawn loot piles containing the dead actors' inventory.\n"
	                         "If disabled, the inventory will be automatically transferred to the killer upon death.";

	        if (ImGui::CollapsingHeader("Looting")) {
	            ImUtil::CheckBox("Player: Spawn Loot Piles",&Settings.bPlayerLootpiles, T0);
	            ImUtil::CheckBox("Followers: Spawn Loot Piles",&Settings.bFollowerLootpiles, T0);
	            ImGui::Spacing();

	        }
	    }

	    ImUtil_Unique {

	        const char* T0 = "Automatically complete this mods' quest.";
	        const char* T1 = "Get the basic size manipulation spells before completing the quest.";
	        const char* T2 = "Instantly complete the perk tree.";

	        if (ImUtil::ConditionalHeader("Skip Progression","Balance Mode Active", !Config::GetBalance().bBalanceMode)) {

	            if (ImUtil::Button("Skip Quest",T0)) {

	            } //TODO Disable by checking quest state
	            ImGui::SameLine();
	            if (ImUtil::Button("Get Basic Spells",T1)) {

	            } //Disable by asking papyrus
	            ImGui::SameLine();
	            if (ImUtil::Button("Get All Perks",T2)) {

	            } //Disable by asking papyrus
	        }
	    }
	}
}