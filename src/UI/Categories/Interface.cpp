#include "UI/Categories/Interface.hpp"
#include "UI/ImGUI/ImUtil.hpp"
#include "UI/ImGUI/ImWindowManager.hpp"
#include "UI/Windows/GTSInfo.hpp"

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

	    //Main Window Settings
	    ImUtil_Unique {

	        const char* T0 = "Automatically handle positioning for this window. You can adjust the position and size offsets below.";
	        const char* T1 = "Adjust the window size as a percentage of the screen.";
	        const char* T2 = "Choose where to align the window on screen.";
	        const char* T3 = "Adjust horizontal offset from the selected anchor point.";
	        const char* T4 = "Adjust vertical offset from the selected anchor point.";

	        if(ImGui::CollapsingHeader("Config Window", ImUtil::HeaderFlags)){ 

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

	            ImGui::EndDisabled();
	            ImGui::Spacing();
	        }
	    }

	    

	}

	void CategoryInterface::DrawRight(){

	    ImUtil_Unique {

	        const char* T0 = "Toggle visibility extra information for the player character.";
	        const char* T1 = "Adjust the opacity of the stats window.";
	        const char* T2 = "Locks the stats window's position and size";
	        const char* T3 = "Choose the alignment point for the window on screen";
	        const char* T4 = "Adjust horizontal offset from the selected anchor point.";
	        const char* T5 = "Adjust vertical offset from the selected anchor point.";

	        if(ImGui::CollapsingHeader("Status Widget",ImUtil::HeaderFlags)){ 
	            ImUtil::CheckBox("Show Player Stats Widget", &Settings.StatusWindow.bVisible, T0);

	            ImUtil::SliderF("Widget Alpha", &Settings.StatusWindow.fAlpha, 0.1f, 1.0f, T1,("%.1fx"));
	            
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