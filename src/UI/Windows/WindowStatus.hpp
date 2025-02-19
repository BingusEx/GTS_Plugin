#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class WindowStatus : public ImWindow {

        public:
	    void CheckFade(RE::Actor* a_actor);
	    void Show();
	    void StartFade();
	    WindowStatus();

        void Draw() override;

        private:

        float AutoFadeAlpha = 1.0f;
	    bool Fading;

	    struct PlayerLastData {
            float Scale;
            float MaxScale;
            float Aspect;
            float Essence;
            double LastWorldTime;
        };

        std::string_view FadeTask = "StatsWindowFadeTask";
        std::string_view ShowTask = "StatsShowTask";

        const float FadeAfter = 7.0f;

        PlayerLastData LastData = {};

        Config& Settings = Config::GetSingleton();
        const SettingsHidden& sHidden = Config::GetHidden();
        const WindowConfStatus& sUI= Config::GetUI().StatusWindow;

        inline bool ShouldShow() override {
            return sUI.bVisible;
        }

        inline float GetAlphaMult() override {
            return sUI.fAlpha * AutoFadeAlpha;
        }

        inline float GetBGAlphaMult() override {
            return sUI.fBGAlphaMult;
        }
    };
}