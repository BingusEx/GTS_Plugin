#include "UI/Categories/Interface.hpp"
#include "UI/ImGUI/ImUtil.hpp"
#include "UI/ImGUI/ImWindowManager.hpp"
#include "UI/Windows/GTSInfo.hpp"
#include "UI/Windows/WindowStatus.hpp"

namespace GTS {

	void CategoryInterface::DrawLeft(){

	    ImUtil_Unique {
	        // --------------------------- UI Settings

	        const char* T0 = "Choose which type of measurement units to display.";

	        if(ImGui::CollapsingHeader("Misc Settings",ImUtil::HeaderFlags)){
	            ImUtil::ComboEx<DisplayUnit>("Measurement Units",Settings.sDisplayUnits, T0);
	            ImGui::Spacing();
	        }
	    }

	    ImUtil_Unique {

	        if(ImGui::CollapsingHeader("UI Settings",ImUtil::HeaderFlags)){

	            const char* T0 = "Adjust the scale of all elements and fonts.";
	            const char* T1 = "Modify the width of UI controls";
	            const char* T2 = "Set the accent color for the UI.";

	            ImUtil::SliderF("UI Scale", &Settings.fScale, 0.7f, 1.8f, T0,"%.1fx");
	            if (ImGui::IsItemDeactivatedAfterEdit()) {
	                StyleMgr.LoadStyle();
	                FontMgr.RebuildFonts();
	            }

	            ImUtil::SliderF("Item Width", &Settings.fItemWidth, 0.4f, 0.7f, T1,"%.2fx");

	            ImGui::ColorEdit3("Accent Color", Settings.f3AccentColor.data(), ImGuiColorEditFlags_DisplayHSV);
	            if (ImGui::IsItemDeactivatedAfterEdit() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])) {
	                StyleMgr.LoadStyle();
	            }
	            if (ImGui::IsItemHovered()){
	                ImGui::SetTooltip(T2);
	            }

	            ImGui::Spacing();
	        }
	    }

	    //------------  Main Window Settings
	    ImUtil_Unique {

	        const char* T0 = "Automatically handle positioning for this window.\n"
							 "Disabling this allows you to move and resize the settings window manually.\n"
							 "Otherwise, if left enabled you can adjust the position and window scale below.";

	        const char* T1 = "Adjust the window size as a percentage of the screen.";
	        const char* T2 = "Choose where to align the window on screen.";
	        const char* T3 = "Adjust horizontal offset from the selected anchor point.";
	        const char* T4 = "Adjust vertical offset from the selected anchor point.";

			const char* T5 = "Adjust the opacity of the stats window.";
			const char* T6 = "Adjust the opacity of the stats window's backround.";
			const char* T7 = "Automatically save settings when closing the settings window.\n"
	    					 "If disabled you have to manually press save each time you modify something for the changes to persist after restarting/reloading the game.";

	        if(ImGui::CollapsingHeader("Config Window", ImUtil::HeaderFlags)){

				ImUtil::CheckBox("Autosave On Close", &Settings.bEnableAutoSaveOnClose, T7);

	            ImUtil::CheckBox("Lock Config Window Position", &Settings.SettingsWindow.bLock, T0);
	            ImGui::BeginDisabled(!Settings.SettingsWindow.bLock);
	            {   
	                ImUtil::SliderF("Window Size", &Settings.SettingsWindow.fWindowSize, 60.0f, 100.0f, T1,"%.0f%%");
	                ImUtil::ComboEx<ImWindow::WindowAnchor>("Anchor", Settings.SettingsWindow.sAnchor, T2);
	                ImGui::BeginDisabled(Settings.SettingsWindow.sAnchor == "kCenter");
	                {
	                    ImUtil::SliderF("Offset (Left/Right)", &Settings.SettingsWindow.f2Offset.at(0), 0.0f, 1280.f, T3,"%.1f");
	                    ImUtil::SliderF("Offset (Up/Down)", &Settings.SettingsWindow.f2Offset.at(1), 0.0f, 720.0f, T4,"%.1f");
	                }
	                ImGui::EndDisabled();
	            }

				ImUtil::SliderF("Window Alpha", &Settings.SettingsWindow.fAlpha, 0.2f, 1.0f, T5, "%.1fx");
				ImUtil::SliderF("Background Alpha", &Settings.SettingsWindow.fBGAlphaMult, 0.2f, 1.0f, T6, "%.1fx");

	            ImGui::EndDisabled();
	            ImGui::Spacing();
	        }
	    }
	}

	void CategoryInterface::DrawRight(){

	    ImUtil_Unique {

	        const char* T0 = "Toggle visibility extra information for the player character.";
	        const char* T1 = "Adjust the opacity of the stats window.";

	        const char* T2 = "Automatically handle positioning and size for the stats window.\n"
							 "Disabling this allows you to move and resize the settings window manually.\n"
							 "Otherwise, if left enabled you can adjust the position offsets below.";

	        const char* T3 = "Choose the alignment point for the window on screen";
	        const char* T4 = "Adjust horizontal offset from the selected anchor point.";
	        const char* T5 = "Adjust vertical offset from the selected anchor point.";
			const char* T6 = "Adjust the opacity of the stats window's backround.";
			const char* T7 = "Toggle whether the stats widget fades if the players scale or max scale don't change for a while.";
			const char* T8 = "Change how long the widget menu should be shown for after a value has changed.";
			const char* T9 = "Set the width of the stats window. Works only if Auto size is disabled.";
			const char* T10 = "Set the height multiplier of the size bar.";
			const char* T11 = "Set the required change (ie Delta) in size for the window to reappear if inactivity fade is enabled.";

	        if(ImGui::CollapsingHeader("Status Widget",ImUtil::HeaderFlags)){ 
				ImUtil::CheckBox("Show Player Stats Widget", &Settings.StatusWindow.bVisible, T0);

				ImUtil::CheckBox("Inactivity Fade", &Settings.StatusWindow.bEnableFade,T7);
				ImUtil::SliderF("Fade After", &Settings.StatusWindow.fFadeAfter, 0.5f, 10.0f, T8, "After %.1f Seconds", !Settings.StatusWindow.bEnableFade);
				ImUtil::SliderF("Reappear Delta", &Settings.StatusWindow.fFadeDelta, 0.0, 0.5f, T11, "After a %.2fx Difference", !Settings.StatusWindow.bEnableFade);
				ImGui::Spacing();

	            ImUtil::SliderF("Widget Alpha", &Settings.StatusWindow.fAlpha, 0.1f, 1.0f, T1,"%.1fx");
				ImUtil::SliderF("Widget BG Alpha", &Settings.StatusWindow.fBGAlphaMult, 0.0f, 1.0f, T6, "%.1fx");
				ImUtil::SliderF("Widget Fixed Width", &Settings.StatusWindow.fFixedWidth, 120.0f, 600.0f, T9, "%.0f");
				ImUtil::SliderF("Size Bar Height Mult", &Settings.StatusWindow.fSizeBarHeightMult, 0.1f, 2.5f, T10, "%.2fx");
	            ImGui::Spacing();
	            
	            ImUtil::CheckBox("Lock Widget Position", &Settings.StatusWindow.bLock, T2);
	            ImGui::BeginDisabled(!Settings.StatusWindow.bLock);
	            {   
	                ImUtil::ComboEx<ImWindow::WindowAnchor>("Anchor", Settings.StatusWindow.sAnchor, T3);
	                ImGui::BeginDisabled(Settings.StatusWindow.sAnchor == "kCenter");
	                {
	                    ImUtil::SliderF("Offset (Left/Right)", &Settings.StatusWindow.f2Offset.at(0), 0.0f, 1280.0f, T4, "%.1f%");
	                    ImUtil::SliderF("Offset (Up/Down)", &Settings.StatusWindow.f2Offset.at(1), 0.0f, 720.0f, T5,"%.1f%");
	                }
	                ImGui::EndDisabled();
	            }
	            ImGui::Spacing();
	            ImGui::EndDisabled();

	            if(ImGui::CollapsingHeader("Widget Flags",ImUtil::HeaderFlags))
	                ImUtil::Bitfield<GTSInfoFeatures>(&Settings.StatusWindow.iFlags);
	        }
	    }
	}
}
