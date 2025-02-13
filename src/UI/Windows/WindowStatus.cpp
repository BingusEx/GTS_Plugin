#include "UI/DearImGui/imgui.h"


#include "magic_enum/magic_enum.hpp"
#include "WindowStatus.hpp"
#include "GTSInfo.hpp"

//categories
#include "UI/ImGui/ImUtil.hpp"
#include "config/ConfigUtil.hpp"

using namespace GtsUI;


inline void DrawInfo(/*Actor* TargetActor */){


    



}


//Do All your Init Stuff here
//Note: Dont do any calls to the imgui api here as the window is not yet created
WindowStatus::WindowStatus() {

    Title = "Player Info";
    Name = "WidgetWindow";
    this->flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;
    AnchorPos = ImWindow::WindowAnchor::kTopRight;
}

void WindowStatus::Draw() {

    DrawBG = hasFlag((GtsUI::GTSInfoFeatures)sUI.iFlags, GtsUI::GTSInfoFeatures::kDrawBackground);
    flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

    //Handle Fixed Position and Size
    if(sUI.bLock){
        //X,Y
        const ImVec2 Offset {sUI.f2Offset[0], sUI.f2Offset[1]};
        ImGui::SetWindowPos(GetAnchorPos(StringToEnum<ImWindow::WindowAnchor>(sUI.sAnchor), Offset));
        
    }
    

    {  
    
        //Get Actor ptr.
        ImGui::PushFont(ImFontManager::GetSingleton().GetFont("widgetbody"));
        DrawGTSInfo((GTSInfoFeatures)Settings.GetUI().StatusWindow.iFlags);
        ImGui::PopFont();

    }

    ImGui::SetWindowSize({0.0f,0.0f});
}


