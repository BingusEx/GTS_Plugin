#include "ImCategoryContainer.hpp"
#include "ImUtil.hpp"
#include "UI/DearImGui/imgui.h"

using namespace GtsUI;

float ImCategoryManager::GetLongestCategory(){

    if(!ImUtil::ValidState()) return -1.0f;

    float longest = 0.0f;
    const float scale = ImGui::GetIO().FontGlobalScale;
    const float paddingX = (ImGui::GetStyle().FramePadding.x + 64.f) * scale;

    for(auto& category : categories){
        if(!category.get()->IsVisible()) continue;
        auto len = ImGui::CalcTextSize(category.get()->GetTitle().c_str());
        longest = std::max(len.x + paddingX ,longest);
    }

    return longest;
}
