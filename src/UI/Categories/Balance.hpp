#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GTS {

    class CategoryBalance: public ImCategorySplit {
        public:
        CategoryBalance(){
            title = "Balance";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsBalance& Settings = Config::GetBalance();
    };

}