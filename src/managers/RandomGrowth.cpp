#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/RandomGrowth.hpp"
#include "magic/effects/common.hpp"
#include "managers/GtsManager.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "data/time.hpp"
#include "timer.hpp"

using namespace RE;
using namespace Gts;

namespace {
	float Get_Breach_Threshold(Actor* actor) {
		float threshold = 1.65;

		if (Runtime::HasPerkTeam(actor, "RandomGrowthTerror")) {
			threshold = 1.60;
		}

		return threshold;
	}

	float Get_size_penalty(Actor* actor) {
		float scale = get_visual_scale(actor);
		float penalty = 1.0;

		if (scale >= 1.5) {
			penalty = std::clamp(scale - 0.5f, 1.0f, 4.0f);
		}

		return penalty;
	}

	bool ShouldGrow(Actor* actor) {
		float MultiplySlider = Runtime::GetFloat("RandomGrowthMultiplyPC");
		if (IsTeammate(actor)) {
			MultiplySlider = Runtime::GetFloat("RandomGrowthMultiplyNPC");
		}
		if (!Runtime::HasPerkTeam(actor, "RandomGrowth") || MultiplySlider <= 0.0) {
			return false;
		}

		if (!IsFemale(actor)) {
			return false;
		}

		if (HasSMT(actor)) {
			return false; // Disallow random groth during Tiny Calamity
		}
		if (SizeManager::GetSingleton().BalancedMode() == 2.0) {
			MultiplySlider = 1.0; // Disable effect in Balance Mode, so slider is always 1.0
		}
		float Gigantism = 1.0 + Ench_Aspect_GetPower(actor);
		int Requirement = ((300 * MultiplySlider * SizeManager::GetSingleton().BalancedMode()) / Gigantism); // Doubles random in Balance Mode
		Requirement *= Get_size_penalty(actor);
		int random = RandomInt(1, Requirement);
		int chance = 1;
		if (random <= chance) {
			return true;
		} else {
			return false;
		}
	}
}

namespace Gts {
	RandomGrowth& RandomGrowth::GetSingleton() noexcept {
		static RandomGrowth instance;
		return instance;
	}

	std::string RandomGrowth::DebugName() {
		return "RandomGrowth";
	}

	void RandomGrowth::Update() {
		static Timer GrowthTimer = Timer(1.0);
		if (GrowthTimer.ShouldRunFrame()) {
			for (auto actor: find_actors()) {
				if (actor && actor->Is3DLoaded()) {
					if (actor->formID == 0x14 || IsTeammate(actor)) {
						if (ShouldGrow(actor)) {
							if (get_target_scale(actor) < get_max_scale(actor)) {
								float scale = get_visual_scale(actor);
								float ProgressionMultiplier = Persistent::GetSingleton().progression_multiplier;
								int random = RandomInt(0, 80);
								float TotalPower = (100.0 + random)/100.0;
								float base_power = ((0.00750 * TotalPower * 25) * ProgressionMultiplier);  // The power of it
								float Gigantism = 1.0 + Ench_Aspect_GetPower(actor);

								if (Runtime::HasPerkTeam(actor, "RandomGrowthAug") && TotalPower >= Get_Breach_Threshold(actor) && !IsGtsBusy(actor)) {
									AnimationManager::StartAnim("StartRandomGrowth", actor);
									return;
								} else {
									ActorHandle gianthandle = actor->CreateRefHandle();
									std::string name = std::format("RandomGrowth_{}", actor->formID);
									// Sounds
									float Volume = std::clamp(scale/4, 0.20f, 1.0f);

									PlayMoanSound(actor, 1.0);
									Task_FacialEmotionTask_Moan(actor, 2.0, "RandomGrow");
									Runtime::PlaySoundAtNode("xlRumble", actor, base_power, 1.0, "NPC COM [COM ]");
									Runtime::PlaySoundAtNode("growthSound", actor, Volume, 1.0, "NPC Pelvis [Pelv]");

									TaskManager::RunFor(name, 0.40 * TotalPower, [=](auto& progressData) {
										if (!gianthandle) {
											return false;
										}
										auto giantref = gianthandle.get().get();
										// Grow
										float delta_time = Time::WorldTimeDelta();
										update_target_scale(giantref, base_power * delta_time * Gigantism, SizeEffectType::kGrow);

										// Play sound
										Rumbling::Once("RandomGrowth", giantref, base_power, 0.10);
										RandomGrowth::RestoreStats(giantref, 0.8); // Regens Attributes if PC has perk
										return true;
									});
								}
							}
						}
					}
				}
			}
		}
	}

	void RandomGrowth::RestoreStats(Actor* actor, float multiplier) { // Regenerate attributes
		float HP = GetMaxAV(actor, ActorValue::kHealth) * 0.00185;
		float MP = GetMaxAV(actor, ActorValue::kMagicka) * 0.00095;
		float SP = GetMaxAV(actor, ActorValue::kStamina) * 0.00125;
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kMagicka, SP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kStamina, MP * TimeScale() * multiplier);
	}
}

