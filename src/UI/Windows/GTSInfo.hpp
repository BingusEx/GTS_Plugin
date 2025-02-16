#pragma once
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"
#include "magic_enum/magic_enum.hpp"

#include "managers/Attributes.hpp"

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

    static inline void DrawGTSInfo(const GTSInfoFeatures a_featureFlags ,RE::Actor* a_Actor){

        if (!a_Actor) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        if (!a_Actor->Get3D()) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        if (!a_Actor->Is3DLoaded()) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        const float CurrentScale = get_visual_scale(a_Actor);
        const float MaxScale = get_max_scale(a_Actor);

        auto& Atrib = AttributeManager::GetSingleton();
        auto transient = Transient::GetSingleton().GetData(a_Actor);

        const float _aspectOfGTS = Ench_Aspect_GetPower(a_Actor) * 100.0f;
        const float _damageResist = (1.0f - Atrib.GetAttributeBonus(a_Actor, ActorValue::kHealth)) * 100.f;
        const float _carryWeight = transient ? transient->carryweight_boost : 0.0f;

        const float _speed = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kSpeedMult) - 1.0f)  * 100.f;
        const float _jumpHeight = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kJumpingBonus) - 1.0f) * 100.0f;
        const float _damage = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kAttackDamageMult) - 1.0f) * 100.0f;

        const std::string sScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format(" ({:.2f}x)", CurrentScale) : "";
        const std::string sReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetFormatedHeight(a_Actor).c_str() : "";
        const std::string pText = fmt::format("{}{}",sReal, sScale);
        ImUtil::CenteredProgress(CurrentScale/MaxScale, ImVec2(hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : 160.0f ,0.0f), pText.c_str());

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowMaxSize))
        ImGui::Text("Max Scale: %.2fx", MaxScale);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowWeight))
        ImGui::Text("Weight: %s", GTS::GetFormatedWeight(a_Actor).c_str());

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowAspect))
        ImGui::Text("Aspect of GTS: %.0f%%", _aspectOfGTS);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgResist))
        ImGui::Text("Bonus Dmg Resist: %.1f%%", _damageResist);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowCarryW))
        ImGui::Text("Bonus Carry Weight: %.1f", _carryWeight);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowSpeedmult))
        ImGui::Text("Bonus Speed: %.1f%%", _speed);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowJumpMult))
        ImGui::Text("Bonus Jump Height: %.1f%%", _jumpHeight);

        if(hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgMult))
        ImGui::Text("Bonus Damage: %.1f%%", _damage);

    }

}

template <>
struct magic_enum::customize::enum_range<GTS::GTSInfoFeatures> {
    static constexpr bool is_flags = true;
};
