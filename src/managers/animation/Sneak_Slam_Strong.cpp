#include "managers/animation/Sneak_Slam_FingerGrind.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/Sneak_Slam_Strong.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/Utils/CrawlUtils.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/CrushManager.hpp"
#include "managers/InputManager.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "ActionSettings.hpp"
#include "data/runtime.hpp"
#include "rays/raycast.hpp"
#include "scale/scale.hpp"


using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace {
    void GTS_Sneak_SlamStrong_Raise_Arm_R(AnimationEventData& data) {
        Utils_UpdateHighHeelBlend(&data.giant, false);
        TrackMatchingHand(&data.giant, CrawlEvent::RightHand, true); // OFF is handled inside Sneak_Slam.cpp
		DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", "DestructionBasics", true, 2.2f);
	} 
	void GTS_Sneak_SlamStrong_Raise_Arm_L(AnimationEventData& data) {
        Utils_UpdateHighHeelBlend(&data.giant, false);
        TrackMatchingHand(&data.giant, CrawlEvent::LeftHand, true); // OFF is handled inside Sneak_Slam.cpp
		DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", "DestructionBasics", true, 2.2f);
	}

    void GTS_Sneak_SlamStrong_Lower_Arm_R(AnimationEventData& data) {}
	void GTS_Sneak_SlamStrong_Lower_Arm_L(AnimationEventData& data) {}

    void GTS_Sneak_SlamStrong_Impact_R(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant);
        DoCrawlingFunctions(&data.giant, scale, 2.75f, Damage_Sneak_HandSlam_Strong, CrawlEvent::RightHand, "RightHandRumble", 1.6f, Radius_Sneak_HandSlam_Strong, 1.0f, DamageSource::HandSlamRight);
        DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", "DestructionBasics", false, 2.2f);
    }  
	void GTS_Sneak_SlamStrong_Impact_L(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant);
        DoCrawlingFunctions(&data.giant, scale, 2.75f, Damage_Sneak_HandSlam_Strong, CrawlEvent::LeftHand, "RightHandRumble", 1.6f, Radius_Sneak_HandSlam_Strong, 1.0f, DamageSource::HandSlamLeft);
        DrainStamina(&data.giant, "StaminaDrain_StrongSneakSlam", "DestructionBasics", false, 2.2f);
        Grab::DamageActorInHand(&data.giant, Damage_Sneak_HandSlam_Strong * 0.6f);
    } 

    void GTS_Sneak_SlamStrong_Impact_Secondary_R(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant) * 0.8f;
        DoCrawlingFunctions(&data.giant, scale, 0.85f, Damage_Sneak_HandSlam_Strong_Secondary, CrawlEvent::RightHand, "RightHandRumble", 0.8f, Radius_Sneak_HandSlam_Strong_Recover, 2.0f, DamageSource::HandSlamRight);
    }
    void GTS_Sneak_SlamStrong_Impact_Secondary_L(AnimationEventData& data) {
        float scale = get_visual_scale(&data.giant) * 0.8f;
        DoCrawlingFunctions(&data.giant, scale, 0.85f, Damage_Sneak_HandSlam_Strong_Secondary, CrawlEvent::LeftHand, "RightHandRumble", 0.8f, Radius_Sneak_HandSlam_Strong_Recover, 2.0f, DamageSource::HandSlamRight);
        Grab::DamageActorInHand(&data.giant, Damage_Sneak_HandSlam_Strong_Secondary * 0.6f);
    }
}

namespace Gts {
    
    void Animation_SneakSlam_Strong::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Raise_Arm_R", "Sneak", GTS_Sneak_SlamStrong_Raise_Arm_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Raise_Arm_L", "Sneak", GTS_Sneak_SlamStrong_Raise_Arm_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Lower_Arm_R", "Sneak", GTS_Sneak_SlamStrong_Lower_Arm_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Lower_Arm_L", "Sneak", GTS_Sneak_SlamStrong_Lower_Arm_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_R", "Sneak", GTS_Sneak_SlamStrong_Impact_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_L", "Sneak", GTS_Sneak_SlamStrong_Impact_L);

        AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_Secondary_R", "Sneak", GTS_Sneak_SlamStrong_Impact_Secondary_R);
		AnimationManager::RegisterEvent("GTS_Sneak_SlamStrong_Impact_Secondary_L", "Sneak", GTS_Sneak_SlamStrong_Impact_Secondary_L);
    }
}