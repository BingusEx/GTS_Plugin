#include "UI/Categories/Advanced.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

    void CategoryAdvanced::DrawLeft() {


        ImUtil_Unique{

            const char* T0 = "Show or hide this page.\n"
							 "After Disabling you have to re-add the option to the settings toml again if you want to re-enable it.";

            if (ImGui::CollapsingHeader("Advanced",ImUtil::HeaderFlags)) {
                ImUtil::CheckBox("Enable/Disable This Page", &Config::GetHidden().IKnowWhatImDoing, T0);
            }
        }

        ImUtil_Unique{

            const char* T0 = "Enable the profiler to diagnose performance issues.\n"
        					 "Note: Needs a game restart to enable after setting it.";
            const char* T1 = "Enable the debug overlay.";

            const char* T2 = "Set the log severity level. The higher it is the more info is dumped into GTSPlugin.log";

            const char* T3 = "Set the flush severity level. The higher it is the more info is dumped into GTSPlugin.log when a crash happens";



            if (ImGui::CollapsingHeader("Logging / Debugging",ImUtil::HeaderFlags)) {

                ImUtil::CheckBox("Enable Profiling",&Settings.bProfile, T0);
                ImUtil::CheckBox("Show Debug Overlay",&Settings.bShowOverlay,T1);

                if (ImUtil::ComboEx<spdlog::level::level_enum>("Log Level", Settings.sLogLevel,T2,false,true)) {
                    spdlog::set_level(spdlog::level::from_str(Settings.sLogLevel));
				}

				if (ImUtil::ComboEx<spdlog::level::level_enum>("Flush Level", Settings.sFlushLevel,T3,false,true)) {
                   spdlog::flush_on(spdlog::level::from_str(Settings.sFlushLevel));
				}

              ImGui::Spacing();

			}

        }

        ImUtil_Unique {

                const char* T0 = "Immediately return from DamageAV Calls for the player.";
                const char* T1 = "Bypass action cooldowns.";
                const char* T2 = "Multiply the resulting GetAnimationSlowdown Value";

                

                if (ImGui::CollapsingHeader("Cheats",ImUtil::HeaderFlags)) {
                    ImUtil::CheckBox("ActorValue Damage",&Settings.bDamageAV, T0);
                    ImUtil::CheckBox("Action Cooldowns",&Settings.bCooldowns, T1);
                    ImUtil::SliderF("Animspeed Player", &Settings.fAnimSpeedAdjMultPlayer, 0.2f, 1.0f, T2);
                    ImUtil::SliderF("Animspeed Teammate", &Settings.fAnimSpeedAdjMultTeammate, 0.2f, 1.0f, T2);

                    ImGui::Spacing();
                }
        }
    }

    void CategoryAdvanced::DrawRight() {

        ImUtil_Unique {

            //ImGui Debug
            if (ImGui::CollapsingHeader("ImGui Debug",ImUtil::HeaderFlags)) {
                {
                    const char* T0 = "Show ImGui's Metrics Window";
                    const char* T1 = "Show ImGui's Stack Window";

                    ImUtil::CheckBox("Show Metrics", &ImWindowManager::GetSingleton().ShowMetrics,T0);
                    ImUtil::CheckBox("Show Stack", &ImWindowManager::GetSingleton().ShowStack,T1);
                }

                if (ImUtil::Button("Quit", "This will immediatly close the game.", false, 1.0f)) {
                    SKSE::WinAPI::TerminateProcess(SKSE::WinAPI::GetCurrentProcess(), EXIT_FAILURE);
                }
            }

        }


        ImUtil_Unique{

            const char* T0 = "Toggle whether to fully pause the game when the menu is open.\n"
                             "It's heavily recomended that you don't disable this.\n"
                             "Applies after closing and re-opening the menu.";

			const char* T1 = "Multiply game speed by this value when the settings menu is open.\nOnly works if Pause game is disabled.";

            //ImGui Debug
            if (ImGui::CollapsingHeader("Pause",ImUtil::HeaderFlags)) {

	            ImUtil::CheckBox("Pause Game", &Settings.bPauseGame, T0);
	            ImUtil::SliderF("SGTM Mult", &Settings.fSGTMMult, 0.05f, 1.0f, T1, "%.2fx", Settings.bPauseGame);

            }

        }

        

    }
}