#include "UI/DearImGui/imgui.h"
#include "UI/Windows/WindowStatus.hpp"
#include "UI/Windows/GTSInfo.hpp"

//categories
#include "Config/ConfigUtil.hpp"
#include "UI/ImGui/ImFontManager.hpp"

namespace  {
	bool AreEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon()) {
        return (fabs(a - b) <= epsilon * std::max(fabs(a), fabs(b)));
    }
}

namespace GTS {

    //Do All your Init Stuff here
    //Note: Dont do any calls to the imgui api here as the window is not yet created

    void WindowStatus::CheckFade(RE::Actor* a_actor) {

        if (!Config::GetUI().StatusWindow.bEnableFade) {
            AutoFadeAlpha = 1.0f;
            return;
        }

        if (a_actor) {

            const float Scale = get_visual_scale(a_actor);
            if (!AreEqual(LastData.Scale, Scale)) {
                LastData.Scale = Scale;
                Show();
                return;
            }

            const float MaxScale = get_max_scale(a_actor);
            if (!AreEqual(LastData.MaxScale, MaxScale)) {
                LastData.MaxScale = MaxScale;
                Show();
                return;
            }

            const float Ench = Ench_Aspect_GetPower(a_actor);
            if (!AreEqual(LastData.Aspect, Ench)) {
                LastData.Aspect = Ench;
                Show();
                return;
            }

            if (AreEqual(AutoFadeAlpha,0.0f)) {
                return;
            }

            if (Time::WorldTimeElapsed() - LastData.LastWorldTime > Config::GetUI().StatusWindow.fFadeAfter) {
                StartFade();
            }
        }
    }


    void WindowStatus::Show() {

        LastData.LastWorldTime = Time::WorldTimeElapsed();

        if (AreEqual(AutoFadeAlpha, 1.0f)) {
            return;
        }

        //Disable Fade
        TaskManager::RunFor(ShowTask, 0.2f, [this](auto& progressData) {
            AutoFadeAlpha = progressData.progress;
            return true;
        });

    }

    void WindowStatus::StartFade() {
        constexpr float Duration = 0.6f;

        if (AreEqual(AutoFadeAlpha, 0.0f)) {
            return;
        }

        TaskManager::RunFor(FadeTask, Duration, [this](auto& progressData) {
        	AutoFadeAlpha = 1.0f - progressData.progress;
			return true;
        });

    }

    WindowStatus::WindowStatus() {

        Title = "Player Info";
        Name = "Status";
        this->flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoFocusOnAppearing;
        AnchorPos = ImWindow::WindowAnchor::kTopRight;
        ConsumeInput = false;
    }

    void WindowStatus::Draw() {

        flags = (sUI.bLock ? (flags | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove) : (flags & ~ImGuiWindowFlags_NoResize & ~ImGuiWindowFlags_NoMove));

        //Handle Fixed Position and Size
        if (sUI.bLock) {
            //X,Y
            const ImVec2 Offset{ sUI.f2Offset[0], sUI.f2Offset[1] };
            ImGui::SetWindowPos(GetAnchorPos(StringToEnum<ImWindow::WindowAnchor>(sUI.sAnchor), Offset));

        }

        {

            if (auto Player = RE::PlayerCharacter::GetSingleton()) {
                //Get Actor ptr.
                ImGui::PushFont(ImFontManager::GetFont("widgetbody"));
                CheckFade(Player);
                DrawGTSInfo(static_cast<GTSInfoFeatures>(Config::GetUI().StatusWindow.iFlags), Player);
                ImGui::PopFont();
            }

        }

        ImGui::SetWindowSize({ 0.0f,0.0f });
    }
}


