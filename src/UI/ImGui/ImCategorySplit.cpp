#include "UI/ImGui/ImCategorySplit.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "Config/Config.hpp"

namespace GTS {

    void ImCategorySplit::Draw() {

        auto WMult = Config::GetUI().fItemWidth;
        {   //Left

            ImGui::BeginChild("SLeft", ImVec2(ImGui::GetContentRegionAvail().x / splitRatio, 0), ImGuiChildFlags_AlwaysUseWindowPadding);
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * WMult);
            DrawLeft();
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }

        ImGui::SameLine(0.0, 8.0f);
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 1.f);
        ImGui::SameLine(0.0, 8.0f);

        {   //Right
            ImGui::BeginChild("SRight", ImVec2(0, 0), ImGuiChildFlags_AlwaysUseWindowPadding);
            ImGui::PushItemWidth(ImGui::GetWindowWidth() * WMult);
            DrawRight();
            ImGui::PopItemWidth();
            ImGui::EndChild();
        }
    }
}