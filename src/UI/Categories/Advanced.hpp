#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GTSUI{

    class CategoryAdvanced: public ImCategorySplit {
        public:
        CategoryAdvanced(){
            title = "Advanced";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAdvanced& Settings = Config::GetSingleton().GetAdvanced();
    };

}