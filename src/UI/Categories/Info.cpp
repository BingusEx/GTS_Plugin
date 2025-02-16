#include "UI/Categories/Info.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/imGui/ImUtil.hpp"
#include "UI/Windows/GTSInfo.hpp"
#include "UI/ImGui/ImFontManager.hpp"
#include "Utils/Text.hpp"

namespace GTS {

    void CategoryInfo::Draw() {

        //Get Curr Follower Count;

        auto FollowerList = FindTeammates();

        const int TempFollowerCount = static_cast<int>(FollowerList.size());

        const float DivWidth = (ImGui::GetContentRegionAvail().x / (TempFollowerCount + 1)) - (ImGui::GetStyle().CellPadding.x + ImGui::GetStyle().FramePadding.x + ImGui::GetStyle().WindowPadding.x);

        ImGui::BeginChild("PlayerInfo", { DivWidth,0 }, true, true);
        {
            auto Player = RE::PlayerCharacter::GetSingleton();

            if (Player) {

                std::string Name = Player->GetName();

                ImGui::PushFont(ImFontManager::GetFont("widgettitle"));

                //Player Singleton -> Get Name;
                ImGui::Text(str_toupper(Name).c_str());

                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);

                ImGui::PopFont();

                ImGui::PushFont(ImFontManager::GetFont("widgetbody"));
                DrawGTSInfo(static_cast<GTSInfoFeatures>(UINT32_MAX), RE::PlayerCharacter::GetSingleton());
                ImGui::PopFont();
            }

        }
        ImGui::EndChild();

        if (TempFollowerCount == 0) return;

        for (auto Follower : FollowerList) {

            if (!Follower)
                continue;
            if (!Follower->Get3D())
                continue;
            if (!Follower->Is3DLoaded())
                continue;

            const std::string Name = Follower->GetName();

            ImUtil::SeperatorV();
            ImGui::BeginChild(Name.c_str(), { DivWidth,0 });

            ImGui::PushFont(ImFontManager::GetFont("widgettitle"));
            ImGui::Text(str_toupper(Name).c_str());
            ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 1.0f);
            ImGui::PopFont();
            ImGui::PushFont(ImFontManager::GetFont("widgetbody"));
            DrawGTSInfo(static_cast<GTSInfoFeatures>(UINT32_MAX), Follower);
            ImGui::PopFont();
            ImGui::EndChild();
        }
    }
}


