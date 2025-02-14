#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "Config/Config.hpp"

namespace GTS {

    class CategoryGameplay: public ImCategorySplit {
        public:
        CategoryGameplay(){
            title = "Gameplay";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsGameplay& Settings = Config::GetGameplay();
        void GameModeOptions(const char* a_title, GameplayActorSettings* a_Settings);
    };

}