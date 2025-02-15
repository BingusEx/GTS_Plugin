#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/perks/PerkHandler.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/GtsSizeManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "magic/effects/common.hpp"
#include "utils/SurvivalMode.hpp"
#include "utils/actorUtils.hpp"
#include "utils/voreUtils.hpp"
#include "managers/Rumble.hpp"
#include "data/persistent.hpp"
#include "data/transient.hpp"
#include "scale/modscale.hpp"
#include "Constants.hpp"
#include "utils/looting.hpp"
#include "managers/vore.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"



#include <cmath>
#include <random>

using namespace RE;
using namespace GTS;

namespace {
    void BuffAttributes(Actor* giant, float tinyscale) {
		if (giant) {
			if (Runtime::HasPerk(giant, "SoulVorePerk")) { // Permamently increases random AV after eating someone
				float TotalMod = 0.33f;
				int Boost = RandomInt(0, 2);
				if (Boost == 0) {
					AddStolenAttributesTowards(giant, ActorValue::kHealth, TotalMod);
				} else if (Boost == 1) {
					AddStolenAttributesTowards(giant, ActorValue::kStamina, TotalMod);
				} else if (Boost >= 2) {
					AddStolenAttributesTowards(giant, ActorValue::kMagicka, TotalMod);
				}
			}
		}
	}

    void UpdateVoreValues(Actor* giant, Actor* tiny, float& Health_Regeneration, float& growth, float& duration) { // Updates values 
        if (Runtime::HasPerkTeam(giant, "VorePerk")) {
			Health_Regeneration = GetMaxAV(tiny, ActorValue::kHealth) * 0.2f; // Default hp/sp regen
		}
		if (Runtime::HasPerkTeam(giant, "Gluttony")) {
			Health_Regeneration = GetMaxAV(tiny, ActorValue::kHealth) * 0.8f; // 4 times stronger hp/sp regen
			duration *= 0.5f; // 50% faster vore, means -50% duration
            growth *= 2.0f; // but 100% more growth
		}
		if (Runtime::HasPerkTeam(giant, "AdditionalGrowth")) {
			growth *= 1.25f; // 25% stronger growth
		}
    }
}

namespace GTS {

	VoreInformation GetVoreInfo(Actor* giant, Actor* tiny, float growth_mult) {
		float recorded_scale = Vore::GetSingleton().ReadOriginalScale(tiny);
		float Health_Regeneration = 0.0f; // No hp regen by default
		float duration = 80.0f; // 80 seconds duration by default
		float growth = 0.275f; // Default power of gaining size

		UpdateVoreValues(giant, tiny, Health_Regeneration, growth, duration);
		std::string_view tiny_name = tiny->GetDisplayFullName();

		float bounding_box = GetSizeFromBoundingBox(tiny);
		float Vore_Power = growth * growth_mult * bounding_box; // power of most buffs that we start

		VoreInformation VoreInfo = VoreInformation { // Create Vore Info, used for Task functions
			.giantess = giant,
			.WasLiving = IsLiving(tiny),
			.Scale = recorded_scale,
			.Vore_Power = Vore_Power,
			.Health_Regeneration = Health_Regeneration,
			.Box_Scale = bounding_box,
			.Duration = duration,
			.Tiny_Name = tiny->GetDisplayFullName(),
		};

		return VoreInfo;
	}

	const float GetGrowthFormula(float a_giantScale, float a_tinyScale, bool a_devourment) {
		const float g = a_giantScale;
		const float t = a_tinyScale;
        const float b = 5.0f; //Dampening factor for GTS formula, change to whatever you think looks good
		const float u = 0.1f; //Dampening factor for DV formula, change to whatever you think looks good
		const float a = g / (g * u);

		if (a_devourment) {
		//https://www.desmos.com/calculator/5abytasrni
			const float out = (t / sqrt(2 * a * sqrt(g * g))) * 1.1f;
			//Notify("DV Gained {} Scale", out);
			return out;
		}

		//https://www.desmos.com/calculator/tpklrq4pxn
		const float out = pow(t, 2.0f / b) / (sqrt(2 * b * sqrt(g * g)));
		//Notify("GTS Gained {} Scale", out);
		return out;
        
	}

    void VoreMessage_SwallowedAbsorbing(Actor* pred, Actor* prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!prey->IsDead() && !Runtime::HasPerk(pred, "SoulVorePerk") || random <= 1) {
				Cprint("{} was Swallowed and is now being slowly absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			} else if (random == 2) {
				Cprint("{} is now absorbing {}", pred->GetDisplayFullName(), prey->GetDisplayFullName());
			} else if (random >= 3) {
				Cprint("{} will soon be completely absorbed by {}", prey->GetDisplayFullName(), pred->GetDisplayFullName());
			}
		}
	}

	void VoreMessage_Absorbed(Actor* pred, std::string_view prey) {
		if (pred) {
			int random = RandomInt(0, 3);
			if (!Runtime::HasPerk(pred, "SoulVorePerk") || random == 0) {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, "SoulVorePerk") && random == 1) {
				Cprint("{} became one with {}", prey, pred->GetDisplayFullName());
			} else if (Runtime::HasPerk(pred, "SoulVorePerk") && random >= 2) {
				Cprint("{} was greedily devoured by {}", prey, pred->GetDisplayFullName());
			} else {
				Cprint("{} was absorbed by {}", prey, pred->GetDisplayFullName());
			}
		}
	}

    void CantVorePlayerMessage(Actor* giant, Actor* tiny, float sizedifference) {
		if (sizedifference < Action_Vore) {
			std::string message = std::format("Player is too big to be eaten: x{:.2f}/{:.2f}", sizedifference, Action_Vore);
			NotifyWithSound(tiny, message);
		}
	}

    void Vore_AdvanceQuest(Actor* pred, Actor* tiny, bool WasDragon, bool WasGiant) {
		if (pred->formID == 0x14 && WasDragon) {
			CompleteDragonQuest(tiny, ParticleType::Blue, false);
			return;
		}
		if (WasGiant) {
			AdvanceQuestProgression(pred, tiny, QuestStage::Giant, 1, true);
		} else {
			AdvanceQuestProgression(pred, tiny, QuestStage::Vore, 1, true);
		}
	}

	void Task_Vore_FinishVoreBuff(const VoreInformation& VoreInfo, int amount_of_tinies, bool Devourment) {
        std::string_view tiny_name = VoreInfo.Tiny_Name; // Used for death message
		
		float sizePower = VoreInfo.Vore_Power;
        float Box_Scale = VoreInfo.Box_Scale; // Base scale of Bounding Box
        float tinySize = VoreInfo.Scale; // Pre-Shrink scale of actor (before entering mouth)
		
		bool WasLiving = VoreInfo.WasLiving;

		Actor* giant = VoreInfo.giantess;

		float multiplier = 1.0f;
		if (Devourment) {
			multiplier = 0.5f; // Because Devourment does it 2 times. Affects Attribute and Weight Gain
		}

        float size_gain = sizePower * 0.5f * GetGrowthFormula(get_visual_scale(giant), tinySize, Devourment);

        if (giant) {
            GainWeight(giant, 3.0f * tinySize * amount_of_tinies * multiplier); // Self explanatory
            ModSizeExperience(giant, 0.20f * multiplier + (tinySize * 0.02f)); // Gain Size Mastery XP

			update_target_scale(giant, size_gain, SizeEffectType::kGrow); // Give GTS Size

            VoreMessage_Absorbed(giant, tiny_name);
            AdjustSizeReserve(giant, size_gain);
            BuffAttributes(giant, tinySize);

            if (giant->formID == 0x14) {
				SurvivalMode_AdjustHunger(giant, tinySize * Box_Scale * multiplier, WasLiving, true);
                AdjustSizeLimit(0.0260f * multiplier, giant);
                AdjustMassLimit(0.0106f * multiplier, giant);
            }
            if (Vore::GetSingleton().GetVoreData(giant).GetTimer() == true) {
				Task_FacialEmotionTask_Moan(giant, 1.0f, "Vore", RandomFloat(0.0f, 0.35f));
                PlayMoanSound(giant, 1.0f); // play timed sound.
            }

            Rumbling::Once("GrowthRumble", giant, 1.75f, 0.30f);
        }
	}

	void Task_Vore_StartVoreBuff(Actor* giant, Actor* tiny, int amount_of_tinies) {
		if (!AllowDevourment()) { // ONLY when using default GTS vore logic, not Devourment one
			std::string name = std::format("Vore_Buff_{}_{}", giant->formID, tiny->formID);
			VoreInformation VoreInfo = GetVoreInfo(giant, tiny, 1.0f);
			ActorHandle gianthandle = giant->CreateRefHandle();
			double start_time = Time::WorldTimeElapsed();

			float giantSize = get_visual_scale(giant);

			float Regeneration = VoreInfo.Health_Regeneration;
			float sizePower = VoreInfo.Vore_Power;
			float Duration = VoreInfo.Duration;
            float tinySize = VoreInfo.Scale;

			float formula = GetGrowthFormula(giantSize, tinySize, false);
			float size_gain = (sizePower * formula * TimeScale()) / 5000;

			TaskManager::Run(name, [=](auto& progressData) {
				if (!gianthandle) {
					return false;
				}
				double timepassed = Time::WorldTimeElapsed() - start_time;
				auto giantref = gianthandle.get().get();
				
				float regen_attributes = GetMaxAV(giantref, ActorValue::kHealth) * 0.0006f;
				float health = std::clamp(Regeneration/4000.0f, 0.0f, regen_attributes);

				DamageAV(giantref, ActorValue::kHealth, -health * TimeScale());
				DamageAV(giantref, ActorValue::kStamina, -health * TimeScale()); 
				// Restore HP and Stamina for GTS

				if (get_target_scale(giantref) < get_max_scale(giantref)) { // For some reason likes to surpass size limit by ~0.03 (multiplies by race scale)
					update_target_scale(giantref, size_gain, SizeEffectType::kGrow);
					AddStolenAttributes(giantref, size_gain * TimeScale());
				}

				if (timepassed >= Duration) {
					Task_Vore_FinishVoreBuff(VoreInfo, amount_of_tinies, false);
					if (giantref->formID == 0x14) {
						shake_camera(giantref, 0.50f, 0.75f);
					}
					return false; // Finish Task
				}

				return true;
			});
		}
	}

	void DevourmentBonuses(Actor* Pred, Actor* Prey, bool Digested, float mult) {
		VoreInformation VoreInfo = GetVoreInfo(Pred, Prey, mult);

		if (Digested) {
			Vore_AdvanceQuest(Pred, Prey, IsDragon(Prey), IsGiant(Prey)); // Progress quest
			if (Pred->formID == 0x14) {
				shake_camera(Pred, 0.50f, 0.75f);
			}
		}

		Task_Vore_FinishVoreBuff(VoreInfo, 1, true);
	}

	void Devourment_Compatibility(Actor* Pred, Actor* Prey, bool Digested) { // Called from the GtsManagerQuest script, takes priority over GTS Vore
		if (Pred && Prey) {
			auto Data = Transient::GetSingleton().GetData(Prey);
			if (Data) {
				bool& Devoured = Data->Devourment_Devoured;
				bool& Eaten = Data->Devourment_Eaten;
				if (!Eaten) { // Stage 1: Health Bar is depleted (actor is dead)
					DevourmentBonuses(Pred, Prey, false, 0.5f); // Value is multiplier of growth power.
					Eaten = true;
				} else if (Digested && !Devoured) { // Stage 2: actor is fully absorbed 
					Notify("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
					Cprint("{} was devoured by {}", Prey->GetDisplayFullName(), Pred->GetDisplayFullName());
					DevourmentBonuses(Pred, Prey, true, 1.5f); // Value is multiplier of growth power.
					Devoured = true;
				}
			}
		}
	}
}