#include "managers/animation/AnimationManager.hpp"
#include "managers/gamemode/GameModeManager.hpp"
#include "magic/effects/TinyCalamity.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/RipClothManager.hpp"
#include "managers/ai/headtracking.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/InputManager.hpp"
#include "managers/Attributes.hpp"
#include "managers/hitmanager.hpp"
#include "managers/highheel.hpp"
#include "data/persistent.hpp"
#include "managers/Rumble.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "utils/debug.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "profiler.hpp"
#include "Config.hpp"
#include "timer.hpp"
#include "node.hpp"
#include <vector>
#include <string>



using namespace GTS;
using namespace RE;
using namespace SKSE;
using namespace std;

namespace {
	const float REDUCTION_FACTOR = 0.44f;
	const float PI = 3.14159f;

	NiPoint3 HeadLocation(TESObjectREFR& obj, const float& scale) {
		NiPoint3 headOffset(0.0f, 0.0f, 0.0f);
		auto location = obj.GetPosition();
		auto asActor = skyrim_cast<Actor*>(&obj);
		if (asActor) {
			auto charCont = asActor->GetCharController();
			if (charCont) {
				headOffset.z = charCont->actorHeight * 70.0f * scale;// * get_natural_scale(asActor, true);
			}
		}
		return location + headOffset;
	}
	NiPoint3 HeadLocation(TESObjectREFR& obj) {
		float scale = 1.0f;
		auto asActor = skyrim_cast<Actor*>(&obj);
		if (asActor) {
			scale = get_visual_scale(asActor);
		}
		return HeadLocation(obj, scale);
	}
	NiPoint3 HeadLocation(TESObjectREFR* obj, const float& scale) {
		if (!obj) {
			return NiPoint3();
		} else {
			return HeadLocation(*obj, scale);
		}
	}
	NiPoint3 HeadLocation(TESObjectREFR* obj) {
		if (!obj) {
			return NiPoint3();
		} else {
			return HeadLocation(*obj);
		}
	}
	NiPoint3 HeadLocation(ActorHandle objRefr, const float& scale) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj, scale);
		}
	}
	NiPoint3 HeadLocation(ActorHandle objRefr) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj);
		}
	}

	NiPoint3 HeadLocation(ObjectRefHandle objRefr, const float& scale) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj, scale);
		}
	}
	NiPoint3 HeadLocation(ObjectRefHandle objRefr) {
		if (!objRefr) {
			return NiPoint3();
		} else {
			auto obj = objRefr.get().get();
			if (!obj) {
				return NiPoint3();
			}
			return HeadLocation(*obj);
		}
	}

	// Rotate spine to look at an actor either leaning back or looking down
	void RotateSpine(Actor* giant, Actor* tiny, HeadtrackingData& data) {
		if (giant->formID == 0x14) {
			return;
		}
		float finalAngle = 0.0f;
		if (tiny) { // giant is the actor that is looking, tiny is the one that is being looked at (Player for example)
			//log::info("Tiny is: {}", tiny->GetDisplayFullName());
			bool Collision_Installed = false; //Detects 'Precision' mod
			float Collision_PitchMult = 0.0f;
			giant->GetGraphVariableBool("Collision_Installed", Collision_Installed);
			if (Collision_Installed == true) {
				giant->GetGraphVariableFloat("Collision_PitchMult", Collision_PitchMult); // If true, obtain value to apply it
				//giant->SetGraphVariableFloat("Collision_PitchMult", 0.0f);
				//log::info("Collision Pitch Mult: {}", Collision_PitchMult);
			}
			auto dialoguetarget = giant->GetActorRuntimeData().dialogueItemTarget;
			if (dialoguetarget) {
				// In dialogue
				if (giant != tiny) { // Just to make sure
					// With valid look at target
					giant->SetGraphVariableBool("GTSIsInDialogue", true); // Allow spine edits
					auto meHead = HeadLocation(giant);
					//log::info("  - meHead: {}", Vector2Str(meHead));
					auto targetHead = HeadLocation(tiny);
					//log::info("  - targetHead: {}", Vector2Str(targetHead));
					auto directionToLook = targetHead - meHead;
					//log::info("  - directionToLook: {}", Vector2Str(directionToLook));
					directionToLook = directionToLook * (1/directionToLook.Length());
					//log::info("  - Norm(directionToLook): {}", Vector2Str(directionToLook));
					NiPoint3 upDirection = NiPoint3(0.0f, 0.0f, 1.0f);
					auto sinAngle = directionToLook.Dot(upDirection);
					//log::info("  - cosAngle: {}", sinAngle);
					auto angleFromUp = fabs(acos(sinAngle) * 180.0f / PI);
					//log::info("  - angleFromUp: {}", angleFromUp);
					float angleFromForward = -(angleFromUp - 90.0f) * REDUCTION_FACTOR;
					//log::info("  - angleFromForward: {}", angleFromForward);

					finalAngle = std::clamp(angleFromForward * REDUCTION_FACTOR, -60.f, 60.f);
					//log::info("  - finalAngle: {}", finalAngle);
				}
			
			} else {
				// Not in dialog
				if (fabs(data.spineSmooth.value) < 1e-3) {
					// Finihed smoothing back to zero
					giant->SetGraphVariableBool("GTSIsInDialogue", false); // Disallow
					//log::info("Setting InDialogue to false");
				}
			}
			//log::info("Pitch Override of {} is {}", giant->GetDisplayFullName(), data.spineSmooth.value);
		}
		data.spineSmooth.target = finalAngle;
		giant->SetGraphVariableFloat("GTSPitchOverride", data.spineSmooth.value);
	}

	/*void RotateCaster(Actor* giant, HeadtrackingData& data) { // Unused
		const float PI = 3.14159f;
		if (!giant) {
			return;
		}
		float finalAngle = 0.0f;
		auto combatController = giant->GetActorRuntimeData().combatController;
		if (combatController) {
			auto& targetHandle = combatController->targetHandle;
			if (targetHandle) {
				auto tiny = targetHandle.get().get();
				if (tiny) {
					//log::info("Combat Target: {}", tiny->GetDisplayFullName());
					auto casterSource = giant->GetMagicCaster(MagicSystem::CastingSource::kLeftHand);
					if (casterSource) {
						auto casterNode = casterSource->GetMagicNode();
						if (casterNode) {
							auto sourceLoc = casterNode->world.translate;
							auto scaleTiny = get_visual_scale(tiny);
							auto targetLoc = HeadLocation(tiny, scaleTiny*0.5f); // 50% up tiny body

							auto directionToLook = targetLoc - sourceLoc;
							//log::info("Combat: Direction: {}", Vector2Str(directionToLook));
							directionToLook = directionToLook * (1/directionToLook.Length());
							NiPoint3 upDirection = NiPoint3(0.0f, 0.0f, 1.0f);
							auto sinAngle = directionToLook.Dot(upDirection);
							auto angleFromUp = fabs(acos(sinAngle));
							float angleFromForward = -(angleFromUp - PI/2.0f);

							//log::info("angleFromForward: {}", angleFromForward);
							finalAngle = std::clamp(angleFromForward, -60.0f * PI /180.0f, 60.f * PI /180.0f);
							//log::info("CasterNode finalAngle: {}", finalAngle);
						}
					}
				}
			}
		}
		// data.casterSmooth.target = finalAngle;
		data.casterSmooth.target = PI*1.5f;

		for (auto casterSourceType: {MagicSystem::CastingSource::kLeftHand, MagicSystem::CastingSource::kRightHand}) {
			auto casterSource = giant->GetMagicCaster(casterSourceType);
			if (casterSource) {
				auto casterNode = casterSource->GetMagicNode();
				if (casterNode) {
					auto targetRotation = NiMatrix3();
					if (data.casterSmooth.value > 1e-3) {
						targetRotation.SetEulerAnglesXYZ(data.casterSmooth.value, 0.0f, 0.0f);
					}
					casterNode->local.rotate = targetRotation;
					casterNode->world.rotate = targetRotation;
					//log::info("Adjusting Node Rotation of {}, target: {}, value: {}", giant->GetDisplayFullName(), targetRotation, casterNode->local.rotate);
					update_node(casterNode);
				}
			}
		}
	}*/
}

namespace GTS {

	Headtracking& Headtracking::GetSingleton() noexcept {
		static Headtracking instance;
		return instance;
	}

	std::string Headtracking::DebugName() {
		return "Headtracking";
	}

	void Headtracking::Update() {
		for (auto actor: find_actors()) {
			this->data.try_emplace(actor->formID);
			if (actor->formID == 0x14 || IsTeammate(actor)) {
				SpineUpdate(actor);
			}
		}
	}

	void Headtracking::SpineUpdate(Actor* me) {
		auto profiler = Profilers::Profile("Headtracking: SpineUpdate");
		if (me->formID == 0x14) {
			return;
		}
		auto ai = me->GetActorRuntimeData().currentProcess;
		Actor* tiny = nullptr;
		if (ai) {
			auto targetObjHandle = ai->GetHeadtrackTarget();
			if (targetObjHandle) {
				auto target = targetObjHandle.get().get();
				auto asActor = skyrim_cast<Actor*>(target);
				if (asActor) {
					tiny = asActor;
				}
			}
		}
		this->data.try_emplace(me->formID);
		RotateSpine(me, tiny, this->data.at(me->formID));
	}
}
