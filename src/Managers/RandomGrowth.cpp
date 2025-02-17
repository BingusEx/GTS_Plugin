#include "Managers/RandomGrowth.hpp"

#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Rumble.hpp"

#include "Magic/effects/common.hpp"

using namespace GTS;

namespace {

	float Get_Breach_Threshold(Actor* actor) {
		float threshold = 1.65f;

		if (Runtime::HasPerkTeam(actor, "RandomGrowthTerror")) {
			threshold = 1.60f;
		}

		return threshold;
	}

	float Get_size_penalty(Actor* actor) {
		float scale = get_visual_scale(actor);
		float penalty = 1.0f;

		if (scale >= 1.5f) {
			penalty = std::clamp(scale - 0.5f, 1.0f, 4.0f);
		}

		return penalty;
	}

	bool ShouldGrow(Actor* actor) {
		float MultiplySlider = Runtime::GetFloat("RandomGrowthMultiplyPC");
		if (IsTeammate(actor)) {
			MultiplySlider = Runtime::GetFloat("RandomGrowthMultiplyNPC");
		}
		if (!Runtime::HasPerkTeam(actor, "RandomGrowth") || MultiplySlider <= 0.0f) {
			return false;
		}

		if (!IsFemale(actor, true)) {
			return false;
		}

		if (HasSMT(actor)) {
			return false; // Disallow random groth during Tiny Calamity
		}
		if (SizeManager::GetSingleton().BalancedMode() == 2.0f) {
			MultiplySlider = 1.0f; // Disable effect in Balance Mode, so slider is always 1.0
		}
		float Gigantism = 1.0f + Ench_Aspect_GetPower(actor);
		int Requirement = static_cast<int>((300 * MultiplySlider * SizeManager::GetSingleton().BalancedMode()) / Gigantism); // Doubles random in Balance Mode
		Requirement = static_cast<int>(Requirement * Get_size_penalty(actor));
		int random = RandomInt(1, Requirement);
		int chance = 1;
		if (random <= chance) {
			return true;
		} else {
			return false;
		}
	}
}

namespace GTS {

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
								float TotalPower = (100.0f + random)/100.0f;
								float base_power = ((0.00750f * TotalPower * 25) * ProgressionMultiplier);  // The power of it
								float Gigantism = 1.0f + Ench_Aspect_GetPower(actor);

								if (Runtime::HasPerkTeam(actor, "RandomGrowthAug") && TotalPower >= Get_Breach_Threshold(actor) && !IsGtsBusy(actor)) {
									AnimationManager::StartAnim("StartRandomGrowth", actor);
								} else {
									if (!IsGrowing(actor)) {
										ActorHandle gianthandle = actor->CreateRefHandle();
										std::string name = std::format("RandomGrowth_{}", actor->formID);
										// Sounds
										float Volume = std::clamp(scale/4, 0.20f, 1.0f);

										if (TotalPower >= 1.45f) {
											PlayMoanSound(actor, 1.0f);
											Task_FacialEmotionTask_Moan(actor, 0.8f, "RandomGrow");
										}
										Runtime::PlaySoundAtNode("xlRumble", actor, base_power, 1.0f, "NPC COM [COM ]");
										Runtime::PlaySoundAtNode("growthSound", actor, Volume, 1.0f, "NPC Pelvis [Pelv]");

										TaskManager::RunFor(name, 0.40f * TotalPower, [=](auto& progressData) {
											if (!gianthandle) {
												return false;
											}
											auto giantref = gianthandle.get().get();
											// Grow
											float delta_time = Time::WorldTimeDelta();
											update_target_scale(giantref, base_power * delta_time * Gigantism, SizeEffectType::kGrow);

											// Play sound
											Rumbling::Once("RandomGrowth", giantref, base_power, 0.10f);
											RandomGrowth::RestoreStats(giantref, 0.8f); // Regens Attributes if PC has perk
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
	}

	void RandomGrowth::RestoreStats(Actor* actor, float multiplier) { // Regenerate attributes
		float HP = GetMaxAV(actor, ActorValue::kHealth) * 0.00185f;
		float MP = GetMaxAV(actor, ActorValue::kMagicka) * 0.00095f;
		float SP = GetMaxAV(actor, ActorValue::kStamina) * 0.00125f;
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, HP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kMagicka, SP * TimeScale() * multiplier);
		actor->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kStamina, MP * TimeScale() * multiplier);
	}
}

