#include "UI/DearImGui/imgui.h"

#include "magic_enum/magic_enum.hpp"
#include "WindowStatus.hpp"
#include "GTSInfo.hpp"

//categories
#include "Config/ConfigUtil.hpp"
#include "UI/ImGui/ImFontManager.hpp"

namespace GTS {

    //Do All your Init Stuff here
    //Note: Dont do any calls to the imgui api here as the window is not yet created
    WindowStatus::WindowStatus() {

        Title = "Player Info";
        Name = "WidgetWindow";
        this->flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs;
        AnchorPos = ImWindow::WindowAnchor::kTopRight;
        ConsumeInput = false;
    }

    void WindowStatus::Draw() {

        DrawBG = hasFlag(static_cast<GTSInfoFeatures>(sUI.iFlags), GTSInfoFeatures::kDrawBackground);
        flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

        //Handle Fixed Position and Size
        if (sUI.bLock) {
            //X,Y
            const ImVec2 Offset{ sUI.f2Offset[0], sUI.f2Offset[1] };
            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<ImWindow::WindowAnchor>(sUI.sAnchor), Offset));

        }

        {

            //Get Actor ptr.
            ImGui::PushFont(ImFontManager::GetFont("widgetbody"));
            DrawGTSInfo(static_cast<GTSInfoFeatures>(Config::GetUI().StatusWindow.iFlags),RE::PlayerCharacter::GetSingleton());
            ImGui::PopFont();

        }

        ImGui::SetWindowSize({ 0.0f,0.0f });
    }
}


