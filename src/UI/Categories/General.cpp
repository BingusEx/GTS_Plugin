#include "UI/Categories/General.hpp"

#include "Config/Keybinds.hpp"

#include "UI/UIManager.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

#include "Utils/QuestUtil.hpp"

namespace GTS {

	void CategoryGeneral::DrawLeft() {

		//------ Protect Actors

	    ImUtil_Unique {

	        const char* T0 = "Protect essential NPCs from being crushed, eaten, or affected by size-related spells/actions.";
	        const char* T1 = "Protect followers from being crushed, eaten, or affected by size-related spells/actions.";

	        if (ImGui::CollapsingHeader("Protect Actors", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Protect Essential NPCs",&Settings.bProtectEssentials, T0);
	            ImUtil::CheckBox("Protect Followers",&Settings.bProtectFollowers, T1);
	            ImGui::Spacing();
	        }
	    }

		//------ Compatibility

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

		//------ Visuals

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

		//------ Experimental

	    ImUtil_Unique {

	        const char* T0 = "Male Actor Support:\n"
	                         "No support is provided for this feature.\n"
	                         "This mod was primarily designed with female NPCs in mind\n"
	                         "and always assumes the player/followers are female.\n"
	                         "Animations may not look good and could cause issues even.\n"
	                         "Use at your own risk.";

			const char* T1 = "Apply computationally expensive damage calculations to all actors.";

	        if (ImGui::CollapsingHeader("Experimental", ImUtil::HeaderFlags)) {
	            ImUtil::CheckBox("Allow Male Actors", &Settings.bEnableMales, T0);
				ImUtil::CheckBox("Apply Size Effects to all Actors", &Settings.bAllActorSizeEffects, T1);
	        	ImGui::Spacing();
	        }
	    }
	}

	void CategoryGeneral::DrawRight() {


		//----- Sneaking

	    ImUtil_Unique {

	        const char* T1 = "Replace sneaking with crawling for the player only.\n(Save specific setting)";
	        const char* T2 = "Replace sneaking with crawling for followers.\n(Save specific setting)";

	        if (ImGui::CollapsingHeader("Sneaking", ImUtil::HeaderFlags)) {
				auto& Persi = Persistent::GetSingleton();

	            ImUtil::CheckBox("Enable Player Crawling", &Persi.EnableCrawlPlayer.value, T1);
	            ImUtil::CheckBox("Enable Follower Crawling", &Persi.EnableCrawlFollower.value, T2);
	            ImGui::Spacing();
	        }
	    }

		//----- Misc

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

	            if (ImUtil::CheckBox("Track Bones During Actions", &Settings.bTrackBonesDuringAnim, T3)) {
					if (!Settings.bTrackBonesDuringAnim) {
						ResetCameraTracking();
					}
	            }

	            ImGui::Spacing();
	        }
	    }

		//----- HH

	    ImUtil_Unique {

	        const char* T0 = "Enable height adjustment/correction for actors wearing high heels.";
	        const char* T1 = "Disable HH height adjustments when using furniture to allow other mods to handle it.";

	        if (ImGui::CollapsingHeader("High-Heels")) {
	            ImUtil::CheckBox("Enable Height Adjustment", &Settings.bEnableHighHeels, T0);

	        	if (ImUtil::CheckBox("Disable When Using Furniture", &Settings.bHighheelsFurniture, T1)){
	            	if (!Settings.bHighheelsFurniture) {

						auto actors = find_actors();

						for (auto actor : actors) {
							if (!actor) {
								return;
							}

							for (bool person : {false, true}) {
								auto npc_root_node = find_node(actor, "NPC", person);
								if (npc_root_node && actor->GetOccupiedFurniture()) {
									npc_root_node->local.translate.z = 0.0f;
									update_node(npc_root_node);
								}
							}
						}
					}
	            }
	        }
	    }


		//------------- Looting

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

		//----------- Skill Tree

		ImUtil_Unique{

			const char* T0 = "Open this mod's custom skill tree";

			if (ImGui::CollapsingHeader("Skill Tree", ImUtil::HeaderFlags)) {
				if (ImUtil::Button("Open Skill Tree",T0)) {
					UIManager::CloseSettings();

				}

				ImGui::Spacing();
			}
		}

		//----------- Progress

	    ImUtil_Unique {

	        const char* T0 = "Automatically complete this mod's quest.";
			const char* T1 = "Get all of the mod's spells";
	        const char* T2 = "Instantly complete the perk tree.";

	        if (ImGui::CollapsingHeader("Skip Progression", ImUtil::HeaderFlags)) {

				const auto Complete = ProgressionQuestCompleted();

	            if (ImUtil::Button("Skip Quest",T0), Complete) {
					SkipProgressionQuest();
	            }

	            ImGui::SameLine();

				if (ImUtil::Button("Get All Spells", T1), !Complete) {
					GiveAllSpellsToPlayer();
				} 

	            ImGui::SameLine();

	            if (ImUtil::Button("Get All Perks",T2), !Complete) {
					GiveAllPerksToPlayer();
	            } 
	        }
	    }

		//-------- Settings Reset

		ImUtil_Unique{

			const char* T0 = "Reset this mod's setting do their default values";

			//Reset
			if (ImGui::CollapsingHeader("Reset Settings")) {
				if (ImUtil::Button("Reset Mod Settings", T0)) {

					Config::GetSingleton().ResetToDefaults();
					Keybinds::GetSingleton().ResetKeybinds();
					ImStyleManager::GetSingleton().LoadStyle();
					ImFontManager::GetSingleton().RebuildFonts();

					spdlog::set_level(spdlog::level::from_str(Config::GetAdvanced().sLogLevel));
					spdlog::flush_on(spdlog::level::from_str(Config::GetAdvanced().sFlushLevel));

					//TODO Add More Stuff Here

					Notify("Mod settins have been reset");
					logger::info("All Mod Settings Reset");
				}
			}
		}
	}
}
