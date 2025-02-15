#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/Stomp_Under_Strong.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/GtsSizeManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "managers/explosion.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "managers/tremor.hpp"
#include "Constants.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"


using namespace std;
using namespace SKSE;
using namespace RE;
using namespace GTS;

namespace {
    const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

    void DoImpactRumble(Actor* giant, std::string_view node, std::string_view name) {
		float shake_power = Rumble_Stomp_Under_Strong;
		float smt = HasSMT(giant) ? 1.5f : 1.0f;
		smt *= GetHighHeelsBonusDamage(giant, true);
		Rumbling::Once(name, giant, shake_power * smt, 0.0f, node, 1.25f);
	}

    void UnderStomp_DoEverything(Actor* giant, float animSpeed, bool right, FootEvent Event, DamageSource Source, std::string_view Node, std::string_view rumble) {
		float perk = GetPerkBonus_Basics(giant);
		float SMT = 1.0f;
		float damage = 1.0f;
		if (HasSMT(giant)) {
			SMT = 1.75f; // Larger Dust
			damage = 1.25f;
		}
		std::string taskname = std::format("StrongUnderStomp_{}", giant->formID);
		ActorHandle giantHandle = giant->CreateRefHandle();

		double Start = Time::WorldTimeElapsed();
		
		TaskManager::RunFor(taskname, 1.0f, [=](auto& update){ // Needed because anim has wrong timing
			if (!giantHandle) {
				return false;
			}

			double Finish = Time::WorldTimeElapsed();
			auto giantref = giantHandle.get().get();

			if (Finish - Start > 0.05) { 
				DoDamageEffect(giantref, Damage_Stomp_Under_Strong * damage * perk, Radius_Stomp_Strong, 8, 0.30f, Event, 1.0f, Source, false);
				DoImpactRumble(giantref, Node, rumble);
				DoDustExplosion(giantref, 1.0f * (SMT), Event, Node);

				DrainStamina(giantref,"StaminaDrain_StrongStomp", "DestructionBasics", false, 5.1f); // 13.5 * 5.1

				DoFootstepSound(giantref, SMT, Event, Node);

				LaunchTask(giantref, 1.05f * perk, 3.60f, Event);

				FootStepManager::DoStrongSounds(giantref, 1.10f + animSpeed/20, Node);
				FootStepManager::PlayVanillaFootstepSounds(giantref, right);

				SetBusyFoot(giantref, BusyFoot::None);
				return false;
			}
			return true;
		});
	}

    void GTS_UnderStomp_CamOn_StrongR(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_StrongStomp", "DestructionBasics", true, 5.1f);
        ManageCamera(&data.giant, true, CameraTracking::R_Foot);
		SetBusyFoot(&data.giant, BusyFoot::RightFoot);
    }

    void GTS_UnderStomp_CamOn_StrongL(AnimationEventData& data) {
        DrainStamina(&data.giant, "StaminaDrain_StrongStomp", "DestructionBasics", true, 5.1f);
        ManageCamera(&data.giant, true, CameraTracking::L_Foot);
		SetBusyFoot(&data.giant, BusyFoot::LeftFoot);
    }

    void GTS_UnderStomp_CamOff_StrongR(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::R_Foot);}
    void GTS_UnderStomp_CamOff_StrongL(AnimationEventData& data) {ManageCamera(&data.giant, false, CameraTracking::L_Foot);}

    void GTS_UnderStomp_Impact_StrongR(AnimationEventData& data) {
		float SavedSpeed = data.animSpeed;
		UnderStomp_DoEverything(&data.giant, SavedSpeed, true, FootEvent::Right, DamageSource::CrushedRight, RNode, "HeavyStompR");
	}
	void GTS_UnderStomp_Impact_StrongL(AnimationEventData& data) {
		float SavedSpeed = data.animSpeed;
		UnderStomp_DoEverything(&data.giant, SavedSpeed, false, FootEvent::Left, DamageSource::CrushedLeft, LNode, "HeavyStompL");
	}
}
namespace GTS {
    void AnimationUnderStompStrong::RegisterEvents() {
        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOn_StrongR", "UnderStomp", GTS_UnderStomp_CamOn_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOn_StrongL", "UnderStomp", GTS_UnderStomp_CamOn_StrongL);

        AnimationManager::RegisterEvent("GTS_UnderStomp_CamOff_StrongR", "UnderStomp", GTS_UnderStomp_CamOff_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_CamOff_StrongL", "UnderStomp", GTS_UnderStomp_CamOff_StrongL);
        

		AnimationManager::RegisterEvent("GTS_UnderStomp_Impact_StrongR", "UnderStomp", GTS_UnderStomp_Impact_StrongR);
		AnimationManager::RegisterEvent("GTS_UnderStomp_Impact_StrongL", "UnderStomp", GTS_UnderStomp_Impact_StrongL);
	}

	void AnimationUnderStompStrong::RegisterTriggers() {
		AnimationManager::RegisterTrigger("UnderStompStrongRight", "Stomp", "GTSBeh_UnderStomp_Start_StrongR");
		AnimationManager::RegisterTrigger("UnderStompStrongLeft", "Stomp", "GTSBeh_UnderStomp_Start_StrongL");
	}
}