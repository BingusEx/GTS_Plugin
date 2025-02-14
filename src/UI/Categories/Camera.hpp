#pragma once

#include "UI/ImGUI/ImCategorySplit.hpp"
#include "config/Config.hpp"

namespace GTSUI{

    class CategoryCamera: public ImCategorySplit {
        public:
        CategoryCamera(){
            title = "Camera";
        }

        void DrawLeft() override;
        void DrawRight() override;

        private:
        SettingsCamera& Settings = Config::GetCamera();
    };

}