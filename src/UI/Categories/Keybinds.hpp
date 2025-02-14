
#pragma once

#include "UI/ImGUI/ImCategory.hpp"
#include "config/Config.hpp"
#include "config/Keybinds.hpp"
#include "UI/ImGui/ImWindowManager.hpp"

namespace GTS {

    class CategoryKeybinds : public ImCategory {
        public:
        CategoryKeybinds(){
            title = "Keybinds";
            for(auto& e : DefaultEvents){
                HeaderStateMap.emplace(e.Event,false);
            }
        }
        void Draw() override;


        private:
        const Config& Settings = Config::GetSingleton();
        Keybinds& KeyMgr = Keybinds::GetSingleton();

        //UI

        void DrawOptions();
        void DrawContent();


        int Div = 2;
        std::string SearchRes;
        bool HideFiltered = false;
        volatile uint8_t ColExpState = 0;

        std::unordered_map<std::string, bool> HeaderStateMap = {};
        //GTSInputEvent Rebinding
        std::string VisualKeyString;
        std::vector<std::string> TempKeys = {};
        int RebindIndex = 0;
        int CurEventIndex = 1000;
        float Width = 0.0f;
        bool DrawInputEvent(GTSInputEvent& Event, std::string a_name);
        const int HeaderFlags =  ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_AutoResizeX ;

    };

}