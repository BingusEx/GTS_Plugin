#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GtsUI{

    class CategoryGeneral: public ImCategorySplit {
        public:
        CategoryGeneral(){
            title = "General";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsGeneral& Settings = Config::GetGeneral();
    };

}