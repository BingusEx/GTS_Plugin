#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/GrabAnimationController.hpp"
#include "managers/emotions/EmotionManager.hpp"
#include "managers/ShrinkToNothingManager.hpp"
#include "managers/damage/SizeHitEffects.hpp"
#include "managers/animation/Grab_Vore.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/CrushManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/Attributes.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/tremor.hpp"
#include "managers/Rumble.hpp"
#include "data/transient.hpp"
#include "Constants.hpp"
#include "managers/vore.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"




#include <random>


using namespace REL;
using namespace GTS;


namespace {
    void GTSGrab_Eat_Start(AnimationEventData& data) {
		auto otherActor = Grab::GetHeldActor(&data.giant);
		auto& VoreData = Vore::GetSingleton().GetVoreData(&data.giant);
		ManageCamera(&data.giant, true, CameraTracking::Grab_Left);
		if (otherActor) {
			VoreData.AddTiny(otherActor);
		}
		StartLHandRumble("GrabVoreL", data.giant, 0.5f, 0.10f);
	}

	void GTSGrab_Eat_OpenMouth(AnimationEventData& data) {
		auto giant = &data.giant;
		auto otherActor = Grab::GetHeldActor(giant);
		auto& VoreData = Vore::GetSingleton().GetVoreData(giant);
		if (otherActor) {
			SetBeingEaten(otherActor, true);
			Vore::GetSingleton().ShrinkOverTime(giant, otherActor, 0.1f);
		}
		Task_FacialEmotionTask_OpenMouth(giant, 1.1f, "GrabVoreOpenMouth", 0.3f);
		StopLHandRumble("GrabVoreL", data.giant);
	}

	void GTSGrab_Eat_Eat(AnimationEventData& data) {
		auto otherActor = Grab::GetHeldActor(&data.giant);
		auto& VoreData = Vore::GetSingleton().GetVoreData(&data.giant);
		if (otherActor) {
			for (auto& tiny: VoreData.GetVories()) {
				if (!AllowDevourment()) {
					VoreData.Swallow();
					if (IsCrawling(&data.giant)) {
						otherActor->SetAlpha(0.0f); // Hide Actor
					}
				} else {
					CallDevourment(&data.giant, otherActor);
				}
			}
		}
	}

	void GTSGrab_Eat_CloseMouth(AnimationEventData& data) {
	}

	void GTSGrab_Eat_Swallow(AnimationEventData& data) {
		auto giant = &data.giant;
		auto otherActor = Grab::GetHeldActor(&data.giant);
		if (otherActor) {
			SetBeingEaten(otherActor, false);
			auto& VoreData = Vore::GetSingleton().GetVoreData(&data.giant);
			for (auto& tiny: VoreData.GetVories()) {
				VoreData.KillAll();
			}
			giant->SetGraphVariableInt("GTS_GrabbedTiny", 0);
			giant->SetGraphVariableInt("GTS_Grab_State", 0);
			Runtime::PlaySoundAtNode("VoreSwallow", &data.giant, 1.0f, 1.0f, "NPC Head [Head]"); // Play sound
			AnimationManager::StartAnim("TinyDied", giant);
			//BlockFirstPerson(giant, false);
			ManageCamera(&data.giant, false, CameraTracking::Grab_Left);
			SetBeingHeld(otherActor, false);
			Grab::DetachActorTask(giant);
			Grab::Release(giant);
		}
	}
}

namespace GTS {
    void Animation_GrabVore::RegisterEvents() {
        AnimationManager::RegisterEvent("GTSGrab_Eat_Start", "Grabbing", GTSGrab_Eat_Start);
		AnimationManager::RegisterEvent("GTSGrab_Eat_OpenMouth", "Grabbing", GTSGrab_Eat_OpenMouth);
		AnimationManager::RegisterEvent("GTSGrab_Eat_Eat", "Grabbing", GTSGrab_Eat_Eat);
		AnimationManager::RegisterEvent("GTSGrab_Eat_CloseMouth", "Grabbing", GTSGrab_Eat_CloseMouth);
		AnimationManager::RegisterEvent("GTSGrab_Eat_Swallow", "Grabbing", GTSGrab_Eat_Swallow);
    }
}