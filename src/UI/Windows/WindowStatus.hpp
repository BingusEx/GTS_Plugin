#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class WindowStatus : public ImWindow {

        public:
        
        WindowStatus();

        void Draw() override;

        private:
        Config& Settings = Config::GetSingleton();
        const SettingsHidden& sHidden = Config::GetHidden();
        const WindowConfStatus& sUI= Config::GetUI().StatusWindow;

        inline bool ShouldShow() override {
            return sUI.bVisible;
        }

        inline float GetAlpha() override {
            return sUI.fAlpha;
        }
    };
}