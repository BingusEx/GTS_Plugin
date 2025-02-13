#pragma once

#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UI/DearImGui/imgui_impl_dx11.h"


#include "config/Config.hpp"

namespace GtsUI {
    class ImFontManager {
        private:

        //Consts
        const std::string _ext = ".ttf";
        const std::string _basePath = "Data\\SKSE\\Plugins\\GTSPlugin\\Fonts\\";

        const std::string _SkyrimGUI_Light = _basePath +"Futura Condensed Light" + _ext;
        const std::string _SkyrimGUI_Medium = _basePath + "Futura Condensed Medium" + _ext;
        const std::string _SkyrimGUI_Console = _basePath + "Arial" + _ext;

        const SettingsUI& Settings = Config::GetUI();

        //Structs
        typedef struct FontData {
            std::string name;
            std::string path;
            float size;
            ImFontConfig* conf = new ImFontConfig();
            ImFont* font;
            FontData(const std::string& name, const std::string& path, float size) : name(name), path(path), size(size) {
                //8 is probably overkill...
                //By directly chaning the font scale this hack is no longer needed.
                //Decrease from default to save a bit of VRAM.
                conf->OversampleH = 2;
                conf->OversampleV = 1;
            }
        } FontData;

        enum AQueueType {
            kRasterizerScale,
            kRebuildAtlas
        };

        //Lists
        std::stack<std::pair<AQueueType, float>> ActionQueue;
        std::unordered_map<std::string, FontData*> Fonts;

        //Funcs
        void ChangeRasterizerScaleImpl(float a_scale);
        void RebuildFontAtlasImpl();
        void BuildFontsInt();

        public:
        ~ImFontManager() = default;

        [[nodiscard]] static inline ImFontManager& GetSingleton() {
            static ImFontManager instance;
            return instance;
        }

        inline void PushAction(AQueueType a_type, const float a_value){
            ActionQueue.push(std::make_pair(a_type, a_value));
        }

        //Queuable Actions
        inline void ChangeRasterizerScale(const float a_scale) {
            PushAction(AQueueType::kRasterizerScale,a_scale);
        }

        inline void RebuildFonts() {
            PushAction(AQueueType::kRebuildAtlas,1.0);
        }

        void Init();
        void AddFont(FontData* a_font);
        [[nodiscard]] static ImFont* GetFont(const std::string& fontName);
        void ProcessActions();

    };
}