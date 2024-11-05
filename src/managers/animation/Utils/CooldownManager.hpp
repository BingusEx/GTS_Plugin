#pragma once
// Module that handles AttributeAdjustment
#include "events.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {

    enum class CooldownSource {
        Damage_Launch,
        Damage_Hand,
        Damage_Thigh,
        Push_Basic,
        Action_ButtCrush,
        Action_HealthGate,
        Action_ScareOther,
        Action_AbsorbOther,
        Action_Breasts_Absorb,
        Action_Breasts_Suffocate,
        Action_Breasts_Vore,
        Action_Hugs,
        Emotion_Laugh,
        Emotion_Moan,
        Misc_RevertSound,
        Misc_GrowthSound,
        Misc_BeingHit,
        Misc_AiGrowth,
        Misc_ShrinkOutburst,
        Misc_ShrinkOutburst_Forced,
        Misc_ShrinkParticle,
        Misc_ShrinkParticle_Animation,
        Misc_TinyCalamityRage,
        Footstep_Right,
        Footstep_Left,
    };

    struct CooldownData {
        double lastPushTime = -1.0e8;
        double lastHandDamageTime = -1.0e8;
        double lastLaunchTime = -1.0e8;
        double lastHealthGateTime = -1.0e8;
        double lastThighDamageTime = -1.0e8;
        double lastButtCrushTime = -1.0e8;
        double lastScareTime = -1.0e8;
        double lastHugTime = -1.0e8;

        double lastAbsorbTime = -1.0e8;

        double lastBreastAbsorbTime = -1.0e8;
        double lastBreastSuffocateTime = -1.0e8;
        double lastBreastVoreTime = -1.0e8;
        
        double lastLaughTime = -1.0e8;
        double lastMoanTime = -1.0e8;

        double lastRevertTime = -1.0e8;
        double lastSoundGrowthTime = -1.0e8;
        double lastHitTime = -1.0e8;
        double lastGrowthTime = -1.0e8;
        double lastOutburstTime = -1.0e8;
        double lastForceOutburstTime = -1.0e8;
        double lastShrinkParticleTime = -1.0e8;
        double lastAnimShrinkParticleTime = -1.0e8;
        double lastTinyCalamityTime = -1.0e8;

        double lastFootstepTime_R = -1.0e8;
        double lastFootstepTime_L = -1.0e8;
    };

    void ApplyActionCooldown(Actor* giant, CooldownSource source);
    float GetRemainingCooldown(Actor* giant, CooldownSource source);
    bool IsActionOnCooldown(Actor* giant, CooldownSource source);

    class CooldownManager : public Gts::EventListener {
		public:
			[[nodiscard]] static CooldownManager& GetSingleton() noexcept;
			virtual std::string DebugName() override;

			virtual void Reset() override;

			CooldownData& GetCooldownData(Actor* actor);

        private: 
			std::map<Actor*, CooldownData> CooldownData;
    };
}