
#pragma once

#include "UI/ImGUI/ImCategory.hpp"
#include "config/Config.hpp"

namespace GtsUI{

    class CategoryInfo : public ImCategory {
        public:
        CategoryInfo(){
            title = "Info";
        }
        void Draw() override;

        private:
        SettingsHidden& Settings = Config::GetHidden();
    };

}