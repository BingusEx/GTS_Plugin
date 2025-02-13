#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "UI/ImGUI/ImCategoryContainer.hpp"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"

#include "config/Config.hpp"
#include "config/Keybinds.hpp"

namespace GtsUI {

    class WindowSettings : public ImWindow {

        public:
        
        WindowSettings();

        void Draw() override;

        inline bool ShouldShow() override {
            return true;
        }

        inline float GetAlpha() override {
            return 1.0f;
        }

        inline void SetDisabled(bool a_Disabled) {
            Disabled = a_Disabled;
        }



        private:
        void AsyncLoad();
        void AsyncSave();

        std::atomic<bool> SaveLoadBusy = false;

        std::string ErrorString = "";
        ImCategoryManager& CatMgr = ImCategoryManager::GetSingleton();
        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();

        Config& Settings = Config::GetSingleton();
        Input::Keybinds& KeyMgr = Input::Keybinds::GetSingleton();
        const SettingsHidden& sHidden = Config::GetHidden();
        const WindowConfSettings& sUI= Config::GetUI().SettingsWindow;
        bool Disabled = false;
    };
}