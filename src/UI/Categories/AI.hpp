#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GTS{

    class CategoryAI: public ImCategorySplit {
        public:
        CategoryAI(){
            title = "AI";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAI& Settings = Config::GetAI();
    };

}