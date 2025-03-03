#include "UI/Categories/General.hpp"
#include "Config/Keybinds.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "UI/UIManager.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "Utils/QuestUtil.hpp"


namespace GTS {

	void CategoryGeneral::DrawLeft() {

		// ----- Animation Check

		ImUtil_Unique {

			const char* T0 = "The automatic check can sometimes be unreliable.\n"
							 "By pressing this you can forcefully try play an animation.\n"
							 "A messagebox should appear stating wether the animation was successfully played or not.";

			if (ImGui::CollapsingHeader("Animations Check", ImUtil::HeaderFlagsDefaultOpen)) {
				const auto Player = PlayerCharacter::GetSingleton();
				const bool WorkingAnims = AnimationsInstalled(Player);

				ImGui::PushFont(ImFontManager::GetFont("widgettitle"));
				ImGui::Text("Animations Installed: ");
				if (WorkingAnims) {
					ImGui::SameLine(0,1);
					ImGui::TextColored(ImUtil::ColorOK,"Yes");
				}
				else {
					ImGui::SameLine(0);
					ImGui::TextColored(ImUtil::ColorError, "No");
				}

				ImGui::PopFont();

				if (ImUtil::Button("Manualy Test Animations", T0)) {

					TaskManager::Run("AnimTestTask", [=](auto& progressData) {

						GTS::UIManager::CloseSettings();

						if (progressData.runtime > 0.2) {

							GTS::AnimationManager::StartAnim("StrongStompRight", Player);

							if (progressData.runtime > 0.5) {

								if (GTS::IsGtsBusy(Player)) {
									RE::DebugMessageBox("Animations are working.");
								}
								else {
									RE::DebugMessageBox("Animations are NOT working.");
								}
								return false;
							}
						}
						return true;
					});
				}
				ImGui::Spacing();
			}
		}

		//------ Protect Actors

	    ImUtil_Unique {

	        const char* T0 = "Protect essential NPCs from being crushed, eaten, or affected by size-related spells/actions.";
	        const char* T1 = "Protect followers from being crushed, eaten, or affected by size-related spells/actions.";

	        if (ImGui::CollapsingHeader("Protect Actors", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Protect Essential NPCs",&Settings.bProtectEssentials, T0);
				ImGui::SameLine();
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

	        if (ImGui::CollapsingHeader("Compatibility", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Devourment Compatibility",&Settings.bDevourmentCompat, T0);
	            ImUtil::CheckBox("Alt Conversation Cam. Compatibility",&Settings.bConversationCamCompat, T1);
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

	        if (ImGui::CollapsingHeader("Experimental", ImUtil::HeaderFlagsDefaultOpen)) {
	            ImUtil::CheckBox("Allow Male Actors", &Settings.bEnableMales, T0);
				ImUtil::CheckBox("Apply Size Effects to all Actors", &Settings.bAllActorSizeEffects, T1);
	        	ImGui::Spacing();
	        }
	    }
	}

	void CategoryGeneral::DrawRight() {


		//----- Misc

	    ImUtil_Unique {

	        const char* T0 = "This toggle enables automatic size adjustment:\n"
	                         "If the player or their followers are too large to fit within a room, they will be temporarily scaled down to roughly 90%% of the room's current height.\n"
	                         "Once outside the small room, they will regrow to their previous size.";

	        const char* T1 = "Adjust the speed of all animations based on an actor's scale.";
			const char* T2 = "Reduce the amount of gore in some sound and visual effects.";


	        if (ImGui::CollapsingHeader("Miscellaneous"), ImUtil::HeaderFlagsDefaultOpen) {
	            ImUtil::CheckBox("Dynamic Size Player", &Settings.bDynamicSizePlayer, T0);
				ImGui::SameLine();
	            ImUtil::CheckBox("Dynamic Size Followers", &Settings.bDynamicSizeFollowers, T0);
	            ImUtil::CheckBox("Dynamic Animation Speed", &Settings.bDynamicAnimspeed, T1);
				ImUtil::CheckBox("Less Gore", &Settings.bLessGore, T2);

	            ImGui::Spacing();
	        }
	    }

		//----- HH

	    ImUtil_Unique {

	        const char* T0 = "Enable height adjustment/correction for actors wearing high heels.";
	        const char* T1 = "Disable HH height adjustments when using furniture to allow other mods to handle it.";

	        if (ImGui::CollapsingHeader("High-Heels"), ImUtil::HeaderFlagsDefaultOpen) {

	            ImUtil::CheckBox("Enable Height Adjustment", &Settings.bEnableHighHeels, T0);

				ImGui::SameLine();

	        	if (ImUtil::CheckBox("Disable When Using Furniture", &Settings.bHighheelsFurniture, T1, !Settings.bEnableHighHeels)){
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

	        if (ImGui::CollapsingHeader("Looting"), ImUtil::HeaderFlagsDefaultOpen) {
	            ImUtil::CheckBox("Player: Spawn Loot Piles",&Settings.bPlayerLootpiles, T0);
				ImGui::SameLine();
	            ImUtil::CheckBox("Followers: Spawn Loot Piles",&Settings.bFollowerLootpiles, T0);
	            ImGui::Spacing();

	        }
	    }

		//----------- Skill Tree

		ImUtil_Unique{

			const char* T0 = "Open this mod's custom skill tree";

			if (ImGui::CollapsingHeader("Skill Tree", ImUtil::HeaderFlagsDefaultOpen)) {
				if (ImUtil::Button("Open Skill Tree",T0)) {
					UIManager::CloseSettings();
					Runtime::SetFloat("OpenGTSSkillMenu", 1.0);
				}

				ImGui::Spacing();
			}
		}

		//----------- Progress

	    ImUtil_Unique {

	        const char* T0 = "Automatically complete this mod's quest.";
			const char* T1 = "Get all of the mod's spells";
	        const char* T2 = "Instantly complete the perk tree.";

	        if (ImGui::CollapsingHeader("Skip Progression")) {

				const auto Complete = ProgressionQuestCompleted();

	            if (ImUtil::Button("Skip Quest",T0, Complete)) {
					SkipProgressionQuest();
	            }

	            ImGui::SameLine();

				if (ImUtil::Button("Get All Spells", T1, !Complete)) {
					GiveAllSpellsToPlayer();
				} 

	            ImGui::SameLine();

	            if (ImUtil::Button("Get All Perks",T2, !Complete)) {
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

					// ----- If You need to do something when settings reset add it here.

					if (!Settings.bTrackBonesDuringAnim) {
						ResetCameraTracking();
					}

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

					Notify("Mod settins have been reset");
					logger::info("All Mod Settings Reset");
				}
			}
		}
	}
}
