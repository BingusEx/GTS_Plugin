#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GTSUI{

    class CategoryAudio: public ImCategorySplit {
        public:
        CategoryAudio(){
            title = "Audio";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsAudio& Settings = Config::GetAudio();
    };
}