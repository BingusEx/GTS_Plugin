#pragma once

#include "UI/ImGUI/ImWindow.hpp"
#include "UI/ImGUI/ImCategoryContainer.hpp"
#include "UI/ImGUI/ImFontManager.hpp"
#include "UI/ImGUI/ImStyleManager.hpp"

#include "Config/Config.hpp"
#include "Config/Keybinds.hpp"

namespace GTS {

    class WindowSettings : public ImWindow {

        public:
        
        WindowSettings();

        void Draw() override;

        inline bool ShouldShow() override {
            return this->Show;
        }


        //Stubs. The settings window does not need these
        inline float GetAlphaMult() override {
            return 1.0f;
        }

        inline float GetBGAlphaMult() override {
            return ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w;
        }

        inline void SetDisabled(bool a_Disabled) {
            Disabled = a_Disabled;
        }

        private:
        void AsyncLoad();
        void AsyncSave();

        std::atomic<bool> SaveLoadBusy = false;

        std::string ErrorString;
        ImCategoryManager& CatMgr = ImCategoryManager::GetSingleton();
        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton();

        Config& Settings = Config::GetSingleton();
        Keybinds& KeyMgr = Keybinds::GetSingleton();
        const SettingsHidden& sHidden = Config::GetHidden();
        const WindowConfSettings& sUI= Config::GetUI().SettingsWindow;
        bool Disabled = false;
    };
}