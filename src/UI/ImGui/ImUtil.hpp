#pragma once

#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_internal.h"


// RAII helper to push an ID on construction and pop it on destruction
// So i don't have to remember to use push pop constantly...
struct ImGuiUniqueID {
    ImGuiUniqueID(int id) { ImGui::PushID(id); }
    ~ImGuiUniqueID() { ImGui::PopID(); }
};

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define ImUtil_Unique \
    if (ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__); true)

#define Imutil_UniqueCall(func_call) \
    ImGuiUniqueID CONCAT(uniqueID_, __COUNTER__)(__COUNTER__); \
    func_call

namespace ImUtil {
    
    //-------------------
    //  Constants
    //-------------------

    //Predefined colors {R, G, B, A} (0.0 to 1.0f)
    constexpr ImVec4 ColorError = {1.0f, 0.35f, 0.30f, 0.9f};
    constexpr ImVec4 ColorSubscript = {1.0f, 1.0f, 1.0f, 0.5f};

    constexpr uint32_t HeaderFlags = ImGuiTreeNodeFlags_DefaultOpen;
    
    constexpr float TooltipDelay = 0.45f; //sec

    [[nodiscard]] static inline bool ValidState() noexcept {
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        return ctx && ctx->WithinFrameScope;
    }

    static void RenderDropShadow(ImTextureID tex_id, float size, ImU8 opacity);

    inline void SeperatorH(){
        ImGui::Spacing();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal, 2.f);
        ImGui::Spacing();
    }

    inline void SeperatorV(){
        ImGui::SameLine();
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 1.f);
        ImGui::SameLine();
    }

    [[nodiscard]] inline ImVec2 ScaleToViewport(float a_Percentage) {
        ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
        //std::clamp(a_Percentage, 10.0f, 100.0f);
        return { viewportSize.x * (a_Percentage / 100.f), viewportSize.y * (a_Percentage / 100.f) };
    }

    const std::string HumanizeString(std::string_view name);

    const bool Button(const char* a_label, const char* a_Tooltip = nullptr, const bool a_disabled = false, const float a_padding = 1.0f);
    const bool CheckBox(const char* a_label, bool* a_state, const char* a_Tooltip = nullptr, const bool a_disabled = false);
    const bool SliderF(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false, const bool a_alwaysclamp = false);
    const bool SliderF2(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false);
    const bool SliderF3(const char* a_label, float* a_value, float a_min, float a_max, const char* a_Tooltip = nullptr, const char* fmt = "%.2f", const bool a_disabled = false);
    
    void CenteredProgress(float fraction, const ImVec2& size_arg, const char* overlay = nullptr);
    
    void Bitfield(const char* a_label, uint32_t* a_bitfield);

    void HelpMarker(const char* a_desc);

    const bool ConditionalHeader(const std::string a_label, const std::string a_ConditionText, const bool a_condition, const bool a_defaultopen = true);

    inline void Tooltip(const char* a_Tip){
        if(!a_Tip) return;
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && GImGui->HoveredIdTimer > TooltipDelay){
            ImGui::SetTooltip(a_Tip);
        }
    }

    template <typename T>
    bool ComboEx(const char* a_label, std::string& currentValue, const char* a_Tooltip = nullptr, bool a_disabled = false, bool a_hasTotal = false) {
        // Retrieve enum metadata
        constexpr auto enumNames = magic_enum::enum_names<T>();
        constexpr auto enumValues = magic_enum::enum_values<T>();

        // Build the items string with pretty-printed names
        std::ostringstream itemsStream;
        const int offset = a_hasTotal ? 2 : 1;
        for (size_t i = 0; i <= enumNames.size() - offset; i++) {
            itemsStream << HumanizeString(enumNames[i]) << '\0';
        }
        std::string items = itemsStream.str();

        // Find current enum index
        auto currentEnum = magic_enum::enum_cast<T>(currentValue);
        int currentIndex = 0;
        
        if (currentEnum.has_value()) {
            // Find the index of the current enum value
            const auto it = std::find(enumValues.begin(), enumValues.end(), currentEnum.value());
            if (it != enumValues.end()) {
                currentIndex = static_cast<int>(std::distance(enumValues.begin(), it));
            }
        } 
        else {
            // Reset to first value if invalid
            currentValue = std::string(enumNames[0]);
        }

        ImGui::BeginDisabled(a_disabled);

        bool res = ImGui::Combo(a_label, &currentIndex, items.c_str());
        Tooltip(a_Tooltip);

        ImGui::EndDisabled();

        if(res){
            T selectedEnum = enumValues[currentIndex];
            currentValue = std::string(magic_enum::enum_name(selectedEnum));
        }

        return res;
    }

    template <typename EnumT>
    void Bitfield(typename std::underlying_type<EnumT>::type* a_bitfield) {
        static_assert(std::is_enum<EnumT>::value, "EnumT must be an enum type");
        
        using Underlying = typename std::underlying_type<EnumT>::type;

        uint32_t itt = 0;
        for (auto flag : magic_enum::enum_values<EnumT>()) {
            Underlying flagValue = static_cast<Underlying>(flag);
            bool bit = ((*a_bitfield) & flagValue) != 0;

            const std::string checkboxLabel = HumanizeString(magic_enum::enum_name(flag));
            ImGui::Checkbox(checkboxLabel.c_str(), &bit);
            
            if(++itt % 3)
                ImGui::SameLine(220.0f * (itt % 3));

            // If the checkbox state has changed, update the bitfield.
            if (bit != (((*a_bitfield) & flagValue) != 0)) {
                *a_bitfield ^= flagValue;
            }
        }
    }



    [[nodiscard]] const bool ContainsString(const std::string& a1, const std::string& a2);

}

