#include "UI/Windows/WindowSettings.hpp"
#include "UI/DearImGui/imgui.h"

//categories
#include "UI/Categories/Gameplay.hpp"
#include "UI/Categories/Info.hpp"
#include "UI/Categories/Interface.hpp"
#include "UI/Categories/Audio.hpp"
#include "UI/Categories/AI.hpp"
#include "UI/Categories/Advanced.hpp"
#include "UI/Categories/Camera.hpp"
#include "UI/Categories/Keybinds.hpp"
#include "UI/Categories/General.hpp"
#include "UI/Categories/Balance.hpp"

#include "UI/ImGui/ImUtil.hpp"
#include "Config/ConfigUtil.hpp"
#include "Managers/Input/InputManager.hpp"

namespace GTS {

	//This is bad... Should probably be disabled.
	//We completely clobber the struct data when re-reading it from the toml's
	//Just one poorly timed memory read and its joever.
	//sadly i cant use atomic types or volatiles in the structs themselves.
	void WindowSettings::AsyncLoad(){

		Plugin::Live();

	    if(!Settings.LoadSettings()){
	        ErrorString = "Could Not Load Settings! Check GTSPlugin.log for more info";
	    }
	    else{
	        ErrorString = "";
	        if(!KeyMgr.LoadKeybinds()){
	            ErrorString = "Could Not Load Input Settings! Check GTSPlugin.log for more info";
	        }
	        else{
	            ErrorString = "";
	        }
	    }

	    
	    StyleMgr.LoadStyle();
	    FontMgr.RebuildFonts();
	    SaveLoadBusy.store(false);
	}

	//Saving doesn't have the same race condition issues that loading has.
	void WindowSettings::AsyncSave(){
	    if(!Settings.SaveSettings()){
	        ErrorString = "Could Not Save Settings! Check GTSPlugin.log for more info.";
	    }
	    else{
	        ErrorString = "";
	        if(!KeyMgr.SaveKeybinds()){
	            ErrorString = "Could Not Save Input Settings! Check GTSPlugin.log for more info.";
	        }
	        else{
	            ErrorString = "";
	        }
	    }

		InputManager::GetSingleton().Init();

	    SaveLoadBusy.store(false);
	}


	//Do All your Init Stuff here
	//Note: Dont do any calls to the imgui api here as the window is not yet created
	WindowSettings::WindowSettings() {

	    Title = "Size Matters - Settings";
	    Name = "Settings";
	    Show = false;
		ConsumeInput = true;
	    flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;

	    //Add Categories
	    CatMgr.AddCategory(std::make_shared<CategoryInfo>());
	    CatMgr.AddCategory(std::make_shared<CategoryGeneral>());
	    CatMgr.AddCategory(std::make_shared<CategoryGameplay>());
	    CatMgr.AddCategory(std::make_shared<CategoryBalance>());
	    CatMgr.AddCategory(std::make_shared<CategoryAudio>());
	    CatMgr.AddCategory(std::make_shared<CategoryAI>());
	    CatMgr.AddCategory(std::make_shared<CategoryCamera>());
	    CatMgr.AddCategory(std::make_shared<CategoryInterface>());
	    CatMgr.AddCategory(std::make_shared<CategoryKeybinds>());
	    CatMgr.AddCategory(std::make_shared<CategoryAdvanced>());
	}

	void WindowSettings::Draw() {

	    auto& Categories = CatMgr.GetCategories();
		ImGui::PushFont(ImFontManager::GetFont("footer"));
	    const float Footer = ImGui::GetFrameHeightWithSpacing() + (ImGui::GetStyle().ItemSpacing.y * 4);  // text + separator
		ImGui::PopFont();
	    
	    //Calc Button Width
	    std::array<const char*,3> Lables = { "Load", "Save", "Reset" };
	    const ImGuiStyle& Style = ImGui::GetStyle();

	    float TotalWidth = Style.ItemSpacing.x; // Add Seperator offset
	    for (auto& Str : Lables){
	        TotalWidth += (ImGui::CalcTextSize(Str).x + 2.0f * Style.FramePadding.x) + Style.ItemSpacing.x;
	    }

	    const float ButtonStartX = ImGui::GetWindowWidth() - TotalWidth - Style.WindowPadding.x;
	    
	    //While mathematically correct 2.0 Just doesn't look right...
	    const float TextCenter = ButtonStartX / 2.0f - ImGui::CalcTextSize(ErrorString.c_str()).x / 2.5f;

	    //Update Window Flags
	    flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

	    //Handle Fixed Position and Size
	    if(sUI.bLock){
	        ImGui::SetWindowSize(ImUtil::ScaleToViewport(sUI.fWindowSize));

	        //Mousedown Check Prevents the window from moving around and messing with the slider while dragging
	        if(!ImGui::GetIO().MouseDown[0]){
	            //X,Y
	            const ImVec2 Offset {sUI.f2Offset[0], sUI.f2Offset[1]};
	            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<ImWindow::WindowAnchor>(sUI.sAnchor), Offset));
	        }
	    }
	    
	    {  // Draw Title

	        ImGui::PushFont(ImFontManager::GetFont("title"));
	        ImGui::Text(Title.c_str());
	        ImGui::PopFont();
	    }
	    
	    ImGui::SameLine(ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize("Control Info (?)").x));
	    
	    {
	        const char* THelp = "1. Holding Ctrl when clicking on a UI element (eg. slider) allows you to manually enter a value instead.\n\n"
								"2. If the settings menu is behaving strangely and you can't select/change things try pressing the Tab key once.";

	        ImGui::TextColored(ImUtil::ColorSubscript, "Help/Info (?)");
	        if (ImGui::IsItemHovered()){
	            ImGui::SetTooltip(THelp);
	        }
	    }

	    ImUtil::SeperatorH();

	    {  // Draw Sidebar

	        ImGui::BeginChild("Sidebar", ImVec2(CatMgr.GetLongestCategory(), ImGui::GetContentRegionAvail().y - Footer), true);
	        ImGui::BeginDisabled(Disabled);
	        ImGui::PushFont(ImFontManager::GetFont("sidebar"));

	        // Display the categories in the sidebar
	        for (uint8_t i = 0; i < Categories.size(); i++) {
	            ImCategory* category = Categories[i].get();

	            //If nullptr / invisible / or dbg category, Do not draw.

	            if(!category) continue;
	            if(!sHidden.IKnowWhatImDoing && category->GetTitle() == "Advanced") continue;
	            if(!category->IsVisible()) continue;

	            if (ImGui::Selectable(category->GetTitle().c_str(), CatMgr.activeIndex == i)) {
	                CatMgr.activeIndex = i;
	            }

	        }

	        ImGui::PopFont();
	        ImGui::EndDisabled();
	        ImGui::EndChild();
	    }

	    ImUtil::SeperatorV();

	    { // Content Area, Where the category contents are drawn

	        ImGui::BeginChild("Content", ImVec2(0, ImGui::GetContentRegionAvail().y - Footer), true); // Remaining width

	        // Validate selectedCategory to ensure it's within bounds
	        if (CatMgr.activeIndex >= 0 && CatMgr.activeIndex < Categories.size()) {
	            ImCategory* selected = Categories[CatMgr.activeIndex].get();
	            selected->Draw(); // Call the Draw method of the selected category
	        } 
	        else {
	            ImGui::TextColored(ImUtil::ColorError,"Invalid category or no categories exist!");
	        }

	        ImGui::EndChild();
	    }

	    ImUtil::SeperatorH();
	    ImGui::BeginDisabled(Disabled);
	    
	    {   //Footer - Mod Info

	        ImGui::PushFont(ImFontManager::GetFont("subscript"));
			const std::string FooterMessage = fmt::format("GTSPlugin {}\nBuild Date: {} {}\nGit SHA1 {}", PluginVersion, __DATE__, __TIME__, git::AnyUncommittedChanges() ? "Unreleased" : git::CommitSHA1().c_str());
	        ImGui::TextColored(ImUtil::ColorSubscript, FooterMessage.c_str());
	        ImGui::PopFont();
	    }

	    ImGui::SameLine(TextCenter);

	    {   
	        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - (Footer / 2.0f) - Style.FramePadding.y);
	        ImGui::PushFont(ImFontManager::GetFont("errortext"));
	        ImGui::PushStyleColor(ImGuiCol_Text,ImUtil::ColorError);
	        ImGui::Text(ErrorString.c_str());
	        ImGui::PopStyleColor();
	        ImGui::PopFont();
	    }

	    ImGui::SameLine(ButtonStartX);
	    
	    {   //-------------  Buttons
	        
	        volatile bool buttonstate = SaveLoadBusy.load();

	        //Load
	        if(ImUtil::Button(Lables[0], "Reload and apply the values currenly stored in Settings.toml and Input.toml", buttonstate, 1.3f)){
	            SaveLoadBusy.store(true);
	            std::thread(&WindowSettings::AsyncLoad, this).detach();
	        }

	        ImGui::SameLine();

	        //Save
	        if(ImUtil::Button(Lables[1], "Save changes to Settings.toml and Input.toml", buttonstate, 1.3f)){
	            SaveLoadBusy.store(true);
	            std::thread(&WindowSettings::AsyncSave, this).detach();
	        }

	        ImUtil::SeperatorV();
	        
	        //Reset
	        if(ImUtil::Button(Lables[2], "Load the default values.\nThis does not delete any previous saved changes unless you explicitly overwrite them by saving.", buttonstate, 1.3f)){
	            Settings.ResetToDefaults();
	            KeyMgr.ResetKeybinds();
	            StyleMgr.LoadStyle();
	            FontMgr.RebuildFonts();
				spdlog::set_level(spdlog::level::from_str(Config::GetAdvanced().sLogLevel));
				spdlog::flush_on(spdlog::level::from_str(Config::GetAdvanced().sFlushLevel));
				logger::info("All Mod Settings Reset");
	        }
	    }
	    ImGui::EndDisabled();
	}
}
