#include "ImFontManager.hpp"
#include "UI/DearImGui/imgui_impl_dx11.h"

using namespace GTSUI;

void ImFontManager::Init(){
    //Initalzie font data, add any new font defines here.
    AddFont( new FontData("sidebar",_SkyrimGUI_Light, 34.0f));
    AddFont( new FontData("title",_SkyrimGUI_Medium, 58.0f));
    AddFont( new FontData("footer",_SkyrimGUI_Medium, 28.0f));
    AddFont( new FontData("text",_SkyrimGUI_Console, 17.0f));
    AddFont( new FontData("errortext",_SkyrimGUI_Console, 21.0f));
    AddFont( new FontData("subscript",_SkyrimGUI_Console, 12.0f));
    AddFont( new FontData("widgetbody",_SkyrimGUI_Light, 22.0f));
    AddFont( new FontData("widgettitle",_SkyrimGUI_Light, 32.0f));

    BuildFontsInt();

    for(auto& [key, value] : Fonts){
        if(value->font == nullptr){
            //logger::critical("Could not load font: {}", key);
            //SKSE Report and fail
        }
    }
}

void ImFontManager::AddFont(FontData* a_font){
    if(a_font) {
        Fonts[a_font->name] = a_font;
    }
}

void ImFontManager::BuildFontsInt(){
    ImGuiIO& io = ImGui::GetIO();
    ImFontAtlas* fontAtlas = io.Fonts;
    fontAtlas->Clear();
    fontAtlas->AddFontDefault();
    for(auto& font : Fonts){
        font.second->font = fontAtlas->AddFontFromFileTTF(font.second->path.c_str(), font.second->size * Settings.fScale, font.second->conf);
    }
}

// Utility function to get a font with fallback
ImFont* ImFontManager::GetFont(const std::string& fontName) {
    auto F = ImFontManager::GetSingleton();
    auto it = F.Fonts.find(fontName);
    if (it != F.Fonts.end()) {
        return it->second->font;  // Font found
    }
    return ImGui::GetIO().Fonts->Fonts[0];    // Fallback to default font
}

void ImFontManager::ProcessActions(){
    if(!ActionQueue.empty()){

        auto [type, value] = ActionQueue.top();
        
        switch (type) {

            case kRasterizerScale:{
                ChangeRasterizerScaleImpl(value);
                break;
            }

            case kRebuildAtlas:{
                RebuildFontAtlasImpl();
                break;
            }

            default:{
                //logger::warn("ImFontmanager: Unimplemented Action!")
                break;
            }
        }
        ActionQueue.pop();
    }
}

void ImFontManager::ChangeRasterizerScaleImpl(float a_scale) {
    for(auto [key, value] : Fonts){
        if(value->font != nullptr){
            value->conf->RasterizerDensity = a_scale;
        }
    }
    RebuildFontAtlasImpl();
}

void ImFontManager::RebuildFontAtlasImpl(){

    BuildFontsInt();
    ImGui::GetIO().Fonts->ClearTexData();
    ImGui::GetIO().Fonts->Build();
    
    //Causes a complete reinit of the imgui context
    //Idealy we'd only want to invalidate the font related stuff
    //That requires a modification to ImGuiDX11, which we can't do... :(
    //As long as you dont spam this it's fine.
    //Even if you did it doesn't cause a memleak. It just lags for ~300ms on each call
    ImGui_ImplDX11_InvalidateDeviceObjects();
}
