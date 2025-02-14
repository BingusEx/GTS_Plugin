#pragma once

#include "ImWindow.hpp"
#include "ImFontManager.hpp"
#include "ImStyleManager.hpp"

namespace GTS {

    class ImWindowManager {

        private:
        std::vector<std::unique_ptr<ImWindow>> windows;
        const std::string wmName = "GTSPluginWM";

        ImFontManager& FontMgr = ImFontManager::GetSingleton();
        ImStyleManager& StyleMgr = ImStyleManager::GetSingleton(); 

        ~ImWindowManager() = default;

        public:

        bool ShowMetrics = false;
        bool ShowStack = false;

        [[nodiscard]] static inline ImWindowManager& GetSingleton() {
            static ImWindowManager instance;
            return instance;
        }

        [[nodiscard]] ImWindow* GetWindowByName(const std::string& a_name);
        
        [[nodiscard]] inline bool HasWindows(){
            return !windows.empty();
        }

        [[nodiscard]] inline bool HasInputConsumers() {
            return std::ranges::any_of(windows, [](const auto& window) {
                return window->Show && window->ConsumeInput;
            });
        }

        [[nodiscard]] inline bool HasActiveWindows() {
            return std::ranges::any_of(windows, [](const auto& window) {
                return window->Show;
            });
        }

        void AddWindow(std::unique_ptr<ImWindow> a_window);
        void Update();

        private:
        ImWindowManager() = default;
        ImWindowManager(const ImWindowManager&) = delete;
        ImWindowManager& operator=(const ImWindowManager&) = delete;
    };

}