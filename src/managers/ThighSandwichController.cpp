#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/ThighSandwich.hpp"
#include "managers/ThighSandwichController.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/GtsSizeManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/explosion.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/tremor.hpp"
#include "managers/Rumble.hpp"
#include "Constants.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"





namespace {
	const float MINIMUM_SANDWICH_DISTANCE = 70.0f;
	const float SANDWICH_ANGLE = 60;
	const float PI = 3.14159f;

	void CantThighSandwichPlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_Sandwich) {
			std::string message = std::format("Player is too big to be sandwiched: x{:.2f}/{:.2f}", sizedifference, Action_Sandwich);
			NotifyWithSound(tiny, message);
		}
	}
}


namespace GTS {
	SandwichingData::SandwichingData(Actor* giant) : giant(giant? giant->CreateRefHandle() : ActorHandle()) {
	}

	void SandwichingData::AddTiny(Actor* tiny) {
		this->tinies.try_emplace(tiny->formID, tiny->CreateRefHandle());
	}

	std::vector<Actor*> SandwichingData::GetActors() {
		std::vector<Actor*> result;
		for (auto& [key, actorref]: this->tinies) {
			auto actor = actorref.get().get();
			result.push_back(actor);
		}
		return result;
	}

	ThighSandwichController& ThighSandwichController::GetSingleton() noexcept {
		static ThighSandwichController instance;
		return instance;
	}

	std::string ThighSandwichController::DebugName() {
		return "ThighSandwichController";
	}

	void SandwichingData::MoveActors(bool move) {
		this->MoveTinies = move;
	}

	void SandwichingData::ManageAi(Actor* giant) {
		if (this->tinies.size() > 0) {
			int random = RandomInt( 0, 20);
			if (random < 9) {
				AnimationManager::StartAnim("ThighAttack", giant);
			} else if (random < 12) {
				AnimationManager::StartAnim("ThighAttack_Heavy", giant);
			}
		} else {
			AnimationManager::StartAnim("ThighExit", giant);
		}
	}

	void SandwichingData::DisableRuneTask(Actor* giant, bool shrink) {
		if (shrink == true) {
			std::string name = std::format("ShrinkRune_{}", giant->formID);
			TaskManager::Cancel(name);
		} else if (shrink == false) {
			std::string name = std::format("ScaleRune_{}", giant->formID);
			TaskManager::Cancel(name);
		}
	}

	void SandwichingData::EnableRuneTask(Actor* giant, bool shrink) {
		string node_name = "GiantessRune";
		if (shrink == true) {
			double Start = Time::WorldTimeElapsed();
			std::string name = std::format("ShrinkRune_{}", giant->formID);
			ActorHandle gianthandle = giant->CreateRefHandle();
			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				double Finish = Time::WorldTimeElapsed();
				auto giantref = gianthandle.get().get();
				auto node = find_node(giantref, node_name, false);
				double timepassed = std::clamp(((Finish - Start) * GetAnimationSlowdown(giantref)) * 0.70, 0.01, 0.98);
				//log::info("Shrink Rune task is running, timepassed: {}, AnimationSlowdown: {} ", timepassed, GetAnimationSlowdown(giantref));
				if (node) {
					node->local.scale = static_cast<float>(std::clamp(1.0 - timepassed, 0.01, 1.0));
					update_node(node);
				}
				if (timepassed >= 0.98f) {
					return false; // end it
				}
				return true;
			});
		} else if (shrink == false) {
			double Start = Time::WorldTimeElapsed();
			std::string name = std::format("ScaleRune_{}", giant->formID);
			ActorHandle gianthandle = giant->CreateRefHandle();
			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				double Finish = Time::WorldTimeElapsed();
				auto giantref = gianthandle.get().get();
				auto node = find_node(giantref, node_name, false);
				double timepassed = std::clamp(((Finish - Start) * GetAnimationSlowdown(giantref)) * 0.80, 0.01, 9999.0);
				//log::info("Grow Rune task is running, timepassed: {}, AnimationSlowdown: {} ", timepassed, GetAnimationSlowdown(giantref));
				if (node) {
					node->local.scale = static_cast<float>(std::clamp(timepassed, 0.01, 1.0));
					update_node(node);
				}
				if (timepassed >= 1.0f) {
					return false; // end it
				}
				return true;
			});
		}
	}

	void SandwichingData::Update() {
		if (this->giant) {
			auto giant = this->giant.get().get();
			bool move = this->MoveTinies;
			if (!giant) {
				return;
			}
			float giantScale = get_visual_scale(giant);
			if (giant->formID != 0x14) {
				if (GetPlayerOrControlled()->formID == 0x14 && this->SandwichTimer.ShouldRun()) {
					this->ManageAi(giant);
				}
			}
			for (auto& [key, tinyref]: this->tinies) {
				if (!move) {
					return;
				}
				auto tiny = tinyref.get().get();
				if (!tiny) {
					return;
				}

				Actor* tiny_is_actor = skyrim_cast<Actor*>(tiny);
				if (tiny_is_actor) {
					ShutUp(tiny_is_actor);
					ForceRagdoll(tiny_is_actor, false);
					AttachToObjectA(giant, tiny_is_actor);
				}

				float tinyScale = get_visual_scale(tiny);
				float sizedifference = GetSizeDifference(giant, tiny, SizeType::VisualScale, true, false);
				float threshold = Action_Sandwich;

				if (giant->IsDead() || sizedifference < threshold || !IsThighSandwiching(giant)) {
					EnableCollisions(tiny);
					SetBeingHeld(tiny, false);
					AllowToBeCrushed(tiny, true);
					PushActorAway(giant, tiny, 1.0f);
					ForceRagdoll(tiny_is_actor, true);
					Cprint("{} slipped out of {} thighs", tiny->GetDisplayFullName(), giant->GetDisplayFullName());
					this->tinies.erase(tiny->formID); // Disallow button abuses to keep tiny when on low scale
				}

				if (this->Suffocate && CanDoDamage(giant, tiny, false)) {
					float sizedifference = giantScale/tinyScale;
					float damage = Damage_ThighSandwich_DOT * sizedifference * TimeScale();
					float hp = GetAV(tiny, ActorValue::kHealth);
					InflictSizeDamage(giant, tiny, damage);
					if (damage > hp && !tiny->IsDead()) {
						this->Remove(tiny);
						PrintSuffocate(giant, tiny);
					}
				}
			}
		}
	}

	void ThighSandwichController::Update() {
		for (auto& [key, SandwichData]: this->data) {
			SandwichData.Update();
		}
	}

	std::vector<Actor*> ThighSandwichController::GetSandwichTargetsInFront(Actor* pred, std::size_t numberOfPrey) {
		// Get vore target for actor
		auto& sizemanager = SizeManager::GetSingleton();
		if (!CanPerformAnimation(pred, AnimationCondition::kGrabAndSandwich)) {
			return {};
		}
		if (IsGtsBusy(pred)) {
			return {};
		}
		if (!pred) {
			return {};
		}
		auto charController = pred->GetCharController();
		if (!charController) {
			return {};
		}

		NiPoint3 predPos = pred->GetPosition();

		auto preys = find_actors();

		// Sort prey by distance
		sort(preys.begin(), preys.end(),
		     [predPos](const Actor* preyA, const Actor* preyB) -> bool
		{
			float distanceToA = (preyA->GetPosition() - predPos).Length();
			float distanceToB = (preyB->GetPosition() - predPos).Length();
			return distanceToA < distanceToB;
		});

		// Filter out invalid targets
		preys.erase(std::remove_if(preys.begin(), preys.end(),[pred, this](auto prey)
		{
			return !this->CanSandwich(pred, prey);
		}), preys.end());

		// Filter out actors not in front
		auto actorAngle = pred->data.angle.z;
		RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };
		RE::NiPoint3 actorForward = RotateAngleAxis(forwardVector, -actorAngle, { 0.f, 0.f, 1.f });

		NiPoint3 predDir = actorForward;
		predDir = predDir / predDir.Length();
		preys.erase(std::remove_if(preys.begin(), preys.end(),[predPos, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - predPos;
			if (preyDir.Length() <= 1e-4) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= 0; // 180 degress
		}), preys.end());

		// Filter out actors not in a truncated cone
		// \      x   /
		//  \  x     /
		//   \______/  <- Truncated cone
		//   | pred |  <- Based on width of pred
		//   |______|
		float predWidth = 70 * get_visual_scale(pred);
		float shiftAmount = fabs((predWidth / 2.0f) / tan(SANDWICH_ANGLE/2.0f));

		NiPoint3 coneStart = predPos - predDir * shiftAmount;
		preys.erase(std::remove_if(preys.begin(), preys.end(),[coneStart, predWidth, predDir](auto prey)
		{
			NiPoint3 preyDir = prey->GetPosition() - coneStart;
			if (preyDir.Length() <= predWidth*0.4f) {
				return false;
			}
			preyDir = preyDir / preyDir.Length();
			float cosTheta = predDir.Dot(preyDir);
			return cosTheta <= cos(SANDWICH_ANGLE*PI/180.0f);
		}), preys.end());

		if (numberOfPrey == 1) {
			return Vore_GetMaxVoreCount(pred, preys);
		}
		
		// Reduce vector size
		if (preys.size() > numberOfPrey) {
			preys.resize(numberOfPrey);
		}

		return preys;
	}

	bool ThighSandwichController::CanSandwich(Actor* pred, Actor* prey) {
		if (pred == prey) {
			return false;
		}
		if (prey->IsDead()) {
			return false;
		}
		if (prey->formID == 0x14 && !Persistent::GetSingleton().vore_allowplayervore) {
			return false;
		}

		if (IsBeingHeld(pred, prey)) {
			return false;
		}

		float pred_scale = get_visual_scale(pred);

		float sizedifference = GetSizeDifference(pred, prey, SizeType::VisualScale, true, false);

		float MINIMUM_SANDWICH_SCALE = Action_Sandwich;

		float MINIMUM_DISTANCE = MINIMUM_SANDWICH_DISTANCE;

		if (HasSMT(pred)) {
			MINIMUM_DISTANCE *= 1.75f;
		}

		float balancemode = SizeManager::GetSingleton().BalancedMode();

		float prey_distance = (pred->GetPosition() - prey->GetPosition()).Length();
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference < MINIMUM_SANDWICH_SCALE) {
			if (pred->formID == 0x14) {
				std::string_view message = fmt::format("{} is too big to be smothered between thighs: x{:.2f}/{:.2f}", prey->GetDisplayFullName(), sizedifference, MINIMUM_SANDWICH_SCALE);
				shake_camera(pred, 0.45f, 0.30f);
				NotifyWithSound(pred, message);
			} else if (this->allow_message && prey->formID == 0x14 && IsTeammate(pred)) {
				CantThighSandwichPlayerMessage(pred, prey, sizedifference);
			}
			return false;
		}
		if (prey_distance <= (MINIMUM_DISTANCE * pred_scale) && sizedifference > MINIMUM_SANDWICH_SCALE) {
			if ((prey->formID != 0x14 && IsEssential(pred, prey))) {
				return false;
			} else {
				return true;
			}
		} else {
			return false;
		}
	}

	void ThighSandwichController::StartSandwiching(Actor* pred, Actor* prey) {
		auto& sandwiching = ThighSandwichController::GetSingleton();
		if (!sandwiching.CanSandwich(pred, prey)) {
			return;
		}
		if (IsBeingHeld(pred, prey)) {
			return;
		}
		
		if (GetSizeDifference(pred, prey, SizeType::VisualScale, false, false) < Action_Sandwich) {
			ShrinkUntil(pred, prey, 6.0f, 0.20f, true);
			return;
		}
		
		auto& data = sandwiching.GetSandwichingData(pred);
		data.AddTiny(prey);
		//BlockFirstPerson(pred, true);
		AnimationManager::StartAnim("ThighEnter", pred);
	}

	void ThighSandwichController::Reset() {
		this->data.clear();
	}
	void SandwichingData::ReleaseAll() {
		this->tinies.clear();
		this->MoveTinies = false;
		this->RuneScale = false;
		this->RuneShrink = false;
	}

	void ThighSandwichController::ResetActor(Actor* actor) {
		this->data.erase(actor->formID);
	}

	void SandwichingData::Remove(Actor* tiny) {
		this->tinies.erase(tiny->formID);
	}

	void SandwichingData::EnableSuffocate(bool enable) {
		this->Suffocate = enable;
	}
	void SandwichingData::ManageScaleRune(bool enable) {
		this->RuneScale = enable;
	}
	void SandwichingData::ManageShrinkRune(bool enable) {
		this->RuneShrink = enable;
	}
	void SandwichingData::OverideShrinkRune(float value) {
		this->ScaleRune.value = value;
	}

	SandwichingData& ThighSandwichController::GetSandwichingData(Actor* giant) {
		// Create it now if not there yet
		this->data.try_emplace(giant->formID, giant);

		return this->data.at(giant->formID);
	}

	void ThighSandwichController::AllowMessage(bool allow) {
		this->allow_message = allow;
	}
}
