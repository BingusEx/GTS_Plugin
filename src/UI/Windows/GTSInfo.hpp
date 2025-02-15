#pragma once
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"
#include "magic_enum/magic_enum.hpp"
#include "utils/UnitConverter.hpp"

namespace GTS {

    const enum class GTSInfoFeatures : uint32_t {
        kUnitScale = (1 << 0),
        kUnitReal  = (1 << 1),
        kShowMaxSize = (1 << 2),
        kShowAspect = (1 << 3),
        kShowWeight = (1 << 4),
        kShowDmgResist = (1 << 5),
        kShowCarryW = (1 << 6),
        kShowSpeedmult = (1 << 7),
        kShowJumpMult = (1 << 8),
        kShowDmgMult = (1 << 9),
        kAutoSize = (1 << 10),
        kDrawBackground = (1 << 11)
    };


    // Enable bitwise operations for the enum
    constexpr GTSInfoFeatures operator|(GTSInfoFeatures lhs, GTSInfoFeatures rhs) {
        return static_cast<GTSInfoFeatures>(
            static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)
        );
    }

    constexpr GTSInfoFeatures operator&(GTSInfoFeatures lhs, GTSInfoFeatures rhs) {
        return static_cast<GTSInfoFeatures>(
            static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)
        );
    }

    constexpr bool hasFlag(GTSInfoFeatures value, GTSInfoFeatures flag) {
        return (value & flag) == flag;
    }

    static inline void DrawGTSInfo(const GTSInfoFeatures a_featureFlags /* ,RE::Actor* a_Actor */){

        //TODO Remove dummy values
        static float CurrentScale = 1.42f;
        static float MaxScale = 10.85f;
        float _aspectOfGTS = 43.0f;
        float _damageResist = 12.0f;
        float _carryWeight = 220.0f;
        float _speed = 113.0f;
        float _jumpHeight = 10.0f;
        float _damage = 141.0f;

 
        const std::string sScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format(" ({:.2f}x)", CurrentScale) : "";
        const std::string sReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetHeight().c_str() : "";
        const std::string pText = fmt::format("{}{}",sReal, sScale);
        ImUtil::CenteredProgress(CurrentScale/MaxScale, ImVec2(hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : 150.0f ,0.0f), pText.c_str());
        

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowMaxSize))
        ImGui::Text("Max Scale: %.2fx", MaxScale);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowWeight))
        ImGui::Text("Weight: %s", GTS::GetWeight().c_str());

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowAspect))
        ImGui::Text("Aspect of GTS: %.0f%%", _aspectOfGTS);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgResist))
        ImGui::Text("Damage Resist: %.1f%%", _damageResist);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowCarryW))
        ImGui::Text("Carry Weight: %.1fu", _carryWeight);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowSpeedmult))
        ImGui::Text("Speed: %.1f%%", _speed);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowJumpMult))
        ImGui::Text("Jump Height: %.1f%%", _jumpHeight);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgMult))
        ImGui::Text("Damage: %.1f%%", _damage);

    }

}

template <>
struct magic_enum::customize::enum_range<GTS::GTSInfoFeatures> {
    static constexpr bool is_flags = true;
};
