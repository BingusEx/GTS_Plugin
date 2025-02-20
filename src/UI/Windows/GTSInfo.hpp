#pragma once

namespace GTS {

    const enum class GTSInfoFeatures : uint32_t {
        kUnitScale = (1 << 0),
        kUnitReal  = (1 << 1),
        kShowMaxSize = (1 << 2),
        kShowAspect = (1 << 3),
        kShowWeight = (1 << 4),
        kShowDmgResist = (1 << 5),
        kShowCarryWeight = (1 << 6),
        kShowSpeedmult = (1 << 7),
        kShowJumpMult = (1 << 8),
        kShowDmgMult = (1 << 9),
        kAutoSize = (1 << 10),
        kShowEssence = (1 << 11),
        kShowBonusSize = (1 << 12),
        kShowShrinkResist = (1 << 13),
        kShowOnTheEdge = (1 << 14),
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

    void DrawGTSInfo(const GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor);
}

template <>
struct magic_enum::customize::enum_range<GTS::GTSInfoFeatures> {
    static constexpr bool is_flags = true;
};
