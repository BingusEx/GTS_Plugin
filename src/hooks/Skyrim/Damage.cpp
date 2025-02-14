#include "Data/persistent.hpp"
#include "Data/runtime.hpp"
#include "Data/Time.hpp"
#include "Data/transient.hpp"
#include "Events.hpp"
#include "Hooks/Detours/FunctionHook.hpp"
#include "Hooks/Skyrim/Damage.hpp"
#include "Managers/animation/Grab.hpp"
#include "Managers/animation/HugShrink.hpp"
#include "Managers/animation/Utils/CooldownManager.hpp"
#include "Managers/damage/SizeHitEffects.hpp"
#include "Managers/OverkillManager.hpp"
#include "Scale/Scale.hpp"
#include "Utils/actorUtils.hpp"
#include "Utils/DifficultyUtils.hpp"


using namespace RE;
using namespace SKSE;

namespace {

	void CameraFOVTask_TP(Actor* actor, PlayerCamera* camera, TempActorData* data, bool AllowEdits) {
		std::string name = std::format("CheatDeath_TP_{}", actor->formID);
		ActorHandle gianthandle = actor->CreateRefHandle();

		if (AllowEdits) {
			camera->worldFOV *= 0.35f;
		}

		float DefaultTP = data->WorldFov_Default;
		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name, [=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}
			auto giantref = gianthandle.get().get();
			double Finish = Time::WorldTimeElapsed();

			if (AllowEdits) {
				camera->worldFOV += DefaultTP * 0.003f;
				if (camera->worldFOV >= DefaultTP) {
					camera->worldFOV = DefaultTP;
					data->IsNotImmune = 1.0f;
					return false; // stop it
				}
			} else {
				double timepassed = Finish - Start;
				if (timepassed > 2.6) {
					data->IsNotImmune = 1.0f;
					return false;
				}
			}
			return true;
		});
	}

	void CameraFOVTask_FP(Actor* actor, PlayerCamera* camera, TempActorData* data, bool AllowEdits) {
		std::string name = std::format("CheatDeath_FP_{}", actor->formID);
		ActorHandle gianthandle = actor->CreateRefHandle();

		camera->firstPersonFOV *= 0.35f;
		float DefaultFP = data->FpFov_Default;

		double Start = Time::WorldTimeElapsed();

		TaskManager::Run(name,[=](auto& progressData) {
			if (!gianthandle) {
				return false;
			}

			auto giantref = gianthandle.get().get();
			double Finish = Time::WorldTimeElapsed();

			if (AllowEdits) {
				camera->firstPersonFOV += DefaultFP * 0.003f;
				if (camera->firstPersonFOV >= DefaultFP) {
					camera->firstPersonFOV = DefaultFP;
					data->IsNotImmune = 1.0f;
					return false; // stop it
				}
			} else {
				double timepassed = Finish - Start;
				if (timepassed > 2.6) {
					data->IsNotImmune = 1.0f;
					return false;
				}
			}
			return true;
		});
	}

	void StartDamageIsNotImmune(Actor* actor) {
		if (actor->formID == 0x14) {
			auto camera = PlayerCamera::GetSingleton();
			if (!camera) {
				return;
			}
			auto AllowEdits = Persistent::GetSingleton().Camera_PermitFovEdits;

			auto tranData = Transient::GetSingleton().GetData(actor);
			bool TP = camera->IsInThirdPerson();
			bool FP = camera->IsInFirstPerson();
			if (tranData) {
				tranData->WorldFov_Default = camera->worldFOV;
				tranData->FpFov_Default = camera->firstPersonFOV;
				tranData->IsNotImmune = 0.0f; // make actor immune to damage
				if (TP) {
					CameraFOVTask_TP(actor, camera, tranData, AllowEdits);
				} else if (FP) {
					CameraFOVTask_FP(actor, camera, tranData, AllowEdits);
				}
			}
		}
	}

	void DoOverkill(Actor* attacker, Actor* receiver, float damage) {
		if (damage > GetMaxAV(receiver, ActorValue::kHealth)) { // Overkill effect
			float size_difference = GetSizeDifference(attacker, receiver, SizeType::VisualScale, true, false);
			if (size_difference >= 12.0f) {
				OverkillManager::GetSingleton().Overkill(attacker, receiver);
			}
		}
	}

	float TinyShield(Actor* receiver) {
		float protection = 1.0f;
		if (receiver->formID == 0x14) {
			auto grabbedActor = Grab::GetHeldActor(receiver);
			if (grabbedActor) {
				protection = 0.75f; // 25% damage reduction
			}
		}
		return protection;
	}

	float HealthGate(Actor* receiver, Actor* attacker, float a_damage) {
		float protection = 1.0f;
		if (receiver->formID == 0x14) {

			a_damage *= GetDifficultyMultiplier(attacker, receiver); // Take difficulty into account

			if (a_damage > GetAV(receiver, ActorValue::kHealth)) {
				if (Runtime::HasPerk(receiver, "HealthGate")) {
					if (!IsActionOnCooldown(receiver, CooldownSource::Action_HealthGate)) {
						ApplyActionCooldown(receiver, CooldownSource::Action_HealthGate);
						float maxhp = GetMaxAV(receiver, ActorValue::kHealth);
						float target = get_target_scale(receiver);
						float natural = get_natural_scale(receiver, true);

						float scale = get_visual_scale(receiver);

						update_target_scale(receiver, -0.35f * scale, SizeEffectType::kShrink);
						if ((target <= natural) || (target - 0.35f * scale <= natural)) {
							set_target_scale(receiver, natural); // to prevent becoming < natural scale
						}
						//Rumbling::For("CheatDeath", receiver, 4.0f, 0.10f, "NPC COM [COM ]", 1.50f, 2.0f);
						Runtime::PlaySound("TriggerHG", receiver, 2.0f, 0.5f);
						shake_camera(receiver, 1.7f, 1.5f);
						
						auto node = find_node(receiver, "NPC Root [Root]");
						if (node) {
							NiPoint3 position = node->world.translate;
							SpawnParticle(receiver, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 5.0f, 7, nullptr); 
							SpawnParticle(receiver, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 4.0f, 7, nullptr); 
							SpawnParticle(receiver, 6.00f, "GTS/Effects/TinyCalamity.nif", NiMatrix3(), position, scale * 3.0f, 7, nullptr); 
						}

						StaggerActor(receiver, attacker, 1.0f);
						StaggerActor(attacker, receiver, 1.0f);
						// stagger each-other

						StartDamageIsNotImmune(receiver);

						Cprint("Health Gate triggered, death avoided");
						Cprint("Damage: {:.2f}, Lost Size: {:.2f}", a_damage, -0.35f * scale);
						Notify("Health Gate triggered, death avoided");
						Notify("Damage: {:.2f}, Lost Size: {:.2f}", a_damage, -0.35f * scale);
						protection = 0.0f;
					}
				}
			}
		}
		if (Runtime::HasPerk(receiver, "DarkArts_Max") && GetHealthPercentage(receiver) <= 0.40f) {
			bool OnCooldown = IsActionOnCooldown(receiver, CooldownSource::Misc_ShrinkOutburst_Forced);
			if (!OnCooldown) {
				ApplyActionCooldown(receiver, CooldownSource::Misc_ShrinkOutburst_Forced);
				ShrinkOutburstExplosion(receiver, true);
			}
		}
		return protection;
	}

	float GrowthDamageResistance(Actor* receiver) {
		float reduction = 1.0f;
		if (IsGrowing(receiver)) {
			int growthtype = 0;
			receiver->GetGraphVariableInt("GTS_Growth_Roll", growthtype);
			if (growthtype > 0) {
				if (Runtime::HasPerk(receiver, "RandomGrowthAug")) {
					reduction -= 0.6f;
				}
				if (Runtime::HasPerk(receiver, "RandomGrowthTerror")) {
					reduction -= 0.25f;
				}
			}
		}
		return reduction;
	}

	float HugDamageResistance(Actor* receiver) {
		float reduction = 1.0f;
		if (HugShrink::GetHuggiesActor(receiver)) {
			if (Runtime::HasPerk(receiver, "HugCrush_ToughGrip")) {
				reduction -= 0.25f; // 25% resistance
			}
			if (Runtime::HasPerk(receiver, "HugCrush_HugsOfDeath")) {
				reduction -= 0.35f; // 35% additional resistance
			}
		}
		return reduction;
	}

	float GetTotalDamageResistance(Actor* receiver, Actor* aggressor) {
		float resistance = GetDamageResistance(receiver) * HugDamageResistance(receiver) * GrowthDamageResistance(receiver);
		float multiplier = GetDamageMultiplier(aggressor) / game_getactorscale(aggressor); // take GetScale into account since it boosts damage as well
		float tiny = 1.0f;
		float IsNotImmune = 1.0f;

		float mult = 1.0f;

		auto transient = Transient::GetSingleton().GetData(receiver);

		if (transient) {
			if (receiver->formID == 0x14) {
				IsNotImmune = transient->IsNotImmune;
				tiny = TinyShield(receiver);
			}
		}
		//log::info("    - Damage Mult: {}, resistance: {}, shield: {}", multiplier, resistance, tiny);
		mult *= (multiplier * resistance * tiny * IsNotImmune);
		return mult;
	}

	void RecordPushForce(Actor* giant, Actor* tiny) {
		// Deal Damage is called earlier than the push so we can just record that
		auto tranData = Transient::GetSingleton().GetData(giant);

        if (tranData) {
			float tiny_scale = get_visual_scale(tiny);
			float giant_scale = get_visual_scale(giant);
			
			if (HasSMT(giant)) {
			    giant_scale *= 2.5f;
		    }

			float difference = giant_scale/tiny_scale;

			float pushResult = 1.0f / (difference*difference*difference);

			float result = std::clamp(pushResult, 0.01f, 1.0f);

            tranData->push_force = result;
			//log::info("Recording Push Force:{} - {}", giant->GetDisplayFullName(), tiny->GetDisplayFullName());
			//log::info("----Value: {}", result);
        } 
	}
}

namespace Hooks {
	
	void Hook_Damage::Hook(Trampoline& trampoline) {

		// SE(5D6300)
		static FunctionHook<void(Actor* a_this, float dmg, Actor* aggressor, uintptr_t maybe_hitdata, TESObjectREFR* damageSrc)> SkyrimTakeDamage(
			RELOCATION_ID(36345, 37335),
			[](auto* a_this, auto dmg, auto* aggressor, uintptr_t maybe_hitdata, auto* damageSrc) { // Universal damage function before Difficulty damage

				if (aggressor) { // apply to hits only, We don't want to decrease fall damage for example
					if (aggressor != a_this) {
						dmg *= GetTotalDamageResistance(a_this, aggressor);
						dmg *= HealthGate(a_this, aggressor, dmg);


						DoOverkill(aggressor, a_this, dmg);
						RecordPushForce(a_this, aggressor);
					}
				}

				SkyrimTakeDamage(a_this, dmg, aggressor, maybe_hitdata, damageSrc);
				return;
			}
		);

		/*static FunctionHook<void(Actor* a_victim, HitData& a_hitData)>ProcessHitHook (      
			 // Affects Damage from Weapon Impacts, sadly can't prevent KillMoves
			 // Affects damage in real-time,  so 40000 becomes 0 for example
			REL::RelocationID(37633, 38586), 
			[](Actor* a_victim, HitData& a_hitData) {
				log::info("ProcessHitHook");
				auto attackerref = a_hitData.aggressor;
				log::info("Victim: {}", a_victim->GetDisplayFullName());
				if (attackerref && a_victim) {
					ProcessHit(attackerref.get().get(), a_victim, a_hitData);
				}
				return ProcessHitHook(a_victim, a_hitData); 
            }
        );*/

		/*static FunctionHook<void(Actor* a_this, uintptr_t param_2, uintptr_t param_3, uintptr_t param_4, uintptr_t param_5)> sub_1406213D0(
			// SE: 1406213D0 : 37525
			RELOCATION_ID(37525, 37525),
			[](auto* a_this, uintptr_t param_2, uintptr_t param_3, uintptr_t param_4, uintptr_t param_5) {
				if (a_this) {
					if (a_this->formID != 0x14) {
						log::info("Actor: {}", a_this->GetDisplayFullName());
						log::info("Param 2: {}", param_2);
						log::info("Param 3: {}", param_3);
						log::info("Param 4: {}", param_4);
						log::info("Param 5: {}", param_5);
					}
				}
				

			
				sub_1406213D0(a_this, param_2, param_3, param_4, param_5);
				return;
			}
		);*/
	}
}
