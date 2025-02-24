#include "Managers/Animation/AnimationManager.hpp"
#include "Papyrus/Plugin.hpp"
#include "Data/Transient.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Attributes.hpp"
#include "Managers/GtsManager.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Managers/Gamemode/GameModeManager.hpp"
#include "Utils/VoreUtils.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GtsPlugin";
	float GetDistanceToCamera(StaticFunctionTag*, Actor* actor) {
		return get_distance_to_camera(actor);
	}
	void SetSizeDamageMultiplier(StaticFunctionTag*, float bonus) {
		Persistent::GetSingleton().size_related_damage_mult = bonus;
	}
	void SetExperienceMultiplier(StaticFunctionTag*, float bonus) {
		Persistent::GetSingleton().experience_mult = bonus;
	}

	void SetLegacySounds(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().legacy_sounds = enabled;
	}

	float GetSizeRelatedDamage(StaticFunctionTag*, Actor* actor, float attribute) {
		SizeAttribute Attribute = static_cast<SizeAttribute>(attribute);
		return SizeManager::GetSingleton().GetSizeAttribute(actor, Attribute);
	}
	float GetSizeVulnerability(StaticFunctionTag*, Actor* actor) {
		return SizeManager::GetSingleton().GetSizeVulnerability(actor);
	}

	float GetStolenAttribute(StaticFunctionTag*, float value) {
		auto player = PlayerCharacter::GetSingleton();
		auto Persistent = Persistent::GetSingleton().GetData(player);
		if (Persistent) {
			if (value == 0) {
				return Persistent->stolen_health;
			} else if (value == 1) {
				return Persistent->stolen_stamin;
			} else if (value == 2) {
				return Persistent->stolen_magick;
			} else if (value == 3) {
				return Persistent->stolen_attributes * 4;
			} else {
				return 0.0f;
			}
			return 0.0f;
		}
		return 0.0f;
	}

	float GetAttributeBonus(StaticFunctionTag*, Actor* actor, float value) {
		auto transient = Transient::GetSingleton().GetData(actor);
		if (!actor) {
			return 1.0f;
		}
		if (!transient) {
			return 1.0f;
		}
		if (value == 1.0f) {
			return AttributeManager::GetSingleton().GetAttributeBonus(actor, ActorValue::kHealth); // Health
		}
		if (value == 2.0f) {
			return AttributeManager::GetSingleton().GetAttributeBonus(actor, ActorValue::kCarryWeight); // Carry Weight
		}
		if (value == 3.0f) {
			return AttributeManager::GetSingleton().GetAttributeBonus(actor, ActorValue::kSpeedMult) - 1.0f; // Speed Multi
		}
		if (value == 4.0f) {
			return AttributeManager::GetSingleton().GetAttributeBonus(actor, ActorValue::kAttackDamageMult) - 1.0f;
		}
		if (value == 5.0f) {
			return AttributeManager::GetSingleton().GetAttributeBonus(actor, ActorValue::kJumpingBonus) - 1.0f;
		}
		return 1.0f;
	}

	float GetFlatAttributeBonus(StaticFunctionTag*, Actor* actor, float value) {
		auto transient = Transient::GetSingleton().GetData(actor);
		if (!actor) {
			return 0.0f;
		}
		if (!transient) {
			return 0.0f;
		}
		if (value == 1.0f) { //get hp
			return transient->health_boost;
		}
		if (value == 2.0f) { // get carry weight
			return transient->carryweight_boost;
		}
		return 0.0f;
	}

	bool ModSizeVulnerability(StaticFunctionTag*, Actor* actor, float amt) {
		if (actor) {
			SizeManager::GetSingleton().ModSizeVulnerability(actor, amt);
			return true;
		}
		return false;
	}

	bool SetGrowthHalfLife(StaticFunctionTag*, Actor* actor, float halflife) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				actor_data->half_life = halflife;
				return true;
			}
		}
		return false;
	}

	float GetGrowthHalfLife(StaticFunctionTag*, Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->half_life;
			}
		}
		return 0.05f;
	}

	bool SetAnimSpeed(StaticFunctionTag*, Actor* actor, float animspeed) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				actor_data->anim_speed = animspeed;
				return true;
			}
		}
		return false;
	}

	// From https://stackoverflow.com/questions/17211122/formatting-n-significant-digits-in-c-without-scientific-notation
	std::string format(double f, int n)
	{
		if (f == 0) {
			return "0";
		}
		int d = (int)::ceil(::log10(f < 0 ? -f : f)); /*digits before decimal point*/
		double order = ::pow(10., n - d);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(std::max(n - d, 0)) << round(f * order) / order;
		return ss.str();
	}

	std::string SigFig(StaticFunctionTag*, float number, int sf) {
		return format(number, sf);
	}

	void SetPlayerStagger(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().allow_stagger = enabled;
	}

	void SetActorAudioOverride(StaticFunctionTag*, bool enabled) {
		log::info("FREQUENCY OVERRIDE: {}", enabled);
		Persistent::GetSingleton().edit_voice_frequency = enabled;
	}

	void SetNPCProtection(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().NPCEffectImmunity = enabled;
	}

	void SetPCProtection(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().PCEffectImmunity = enabled;
	}

	void SetWeightGain(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().allow_weight_gain = enabled;
	}

	void EnableCollisionLayerAndMotion(StaticFunctionTag*, TESObjectREFR* ref) {
		if (!ref) {
			return;
		}
		auto current3D = ref->GetCurrent3D();
		if (!current3D) {
			return; // Retry next frame
		}
		current3D->SetMotionType((uint32_t)hkpMotion::MotionType::kCharacter, true, true, true);
		current3D->SetCollisionLayer(COL_LAYER::kCharController);
	}
	void DisableCollisionLayerAndMotion(StaticFunctionTag*, TESObjectREFR* ref) {
		if (!ref) {
			return;
		}
		auto current3D = ref->GetCurrent3D();
		if (!current3D) {
			return; // Retry next frame
		}
		current3D->SetMotionType((uint32_t)hkpMotion::MotionType::kKeyframed, true, true, true);
		current3D->SetCollisionLayer(COL_LAYER::kNonCollidable);
	}

	void ResetQuestProgression(StaticFunctionTag*) {
		ResetQuest();
	}

	float Quest_GetProgression(StaticFunctionTag*, int stage) {
		return GetQuestProgression(stage);
	}

	float GetAspectOfGiantessPower(StaticFunctionTag*) {
		auto player = PlayerCharacter::GetSingleton();
		return Ench_Aspect_GetPower(player);
	}

	void SetAllowPlayerVore(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().vore_allowplayervore = enabled;
	}

	void SetInsectVore(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().AllowInsectVore = enabled;
	}

	void SetUndeadVore(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().AllowUndeadVore = enabled;
	}

	void DistributeRandomItems(StaticFunctionTag*) {
		//DistributeChestItems();
	}

	void SetOnlyCombatVore(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().vore_combatonly = enabled;
	}

	void IncreaseSizeLimit(StaticFunctionTag*, float value, Actor* caster) {
		AdjustSizeLimit(value, caster);
	}

	void IncreaseMassLimit(StaticFunctionTag*, float value, Actor* caster) {
		AdjustMassLimit(value, caster);
	}

	void AdjustBalanceMode(StaticFunctionTag*, int parameter, float modifier) {
		BalanceModeInfo param = static_cast<BalanceModeInfo>(parameter);
		auto& Persist = Persistent::GetSingleton();
		switch (param) {
			case BalanceModeInfo::SizeGain_Penalty: // 1.0
				Persist.BalanceMode_SizeGain_Penalty = modifier;
			break;
			case BalanceModeInfo::ShrinkRate_Base: // 1.0
				Persist.BalanceMode_ShrinkRate_Base = modifier;
			break;
			case BalanceModeInfo::ShrinkRate_Combat: // 0.08
				Persist.BalanceMode_ShrinkRate_Combat = modifier;
			break;
		}
	}

	void SetProgressionMultiplier(StaticFunctionTag*, float value) {
		Persistent::GetSingleton().progression_multiplier = value;
	}

	void SetStompAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Stomp_Ai = enabled;
	}

	void SetSandwichAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Sandwich_Ai = enabled;
	}
	void SetFollowerInteractions(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().FollowerInteractions = enabled;
	}

	void SetVoreAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Vore_Ai = enabled;
	}
	void SetHugsAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Hugs_Ai = enabled;
	}
	void SetThighAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Thigh_Ai = enabled;
	}
	void SetKickAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Kick_Ai = enabled;
	}
	void SetButtCrushAi(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().Butt_Ai = enabled;
	}

	void SetActorPanic(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().actors_panic = enabled;
	}

	void ToggleHostileDamage(StaticFunctionTag*, bool enabled) {
		Persistent::GetSingleton().hostile_toggle = enabled;
	}

	void DisintegrateTarget(StaticFunctionTag*, Actor* actor) {
		if (actor) {
			Disintegrate(actor);
		}
	}

	bool WasDragonEaten(StaticFunctionTag*) {
		auto pc = PlayerCharacter::GetSingleton();
		auto transient = Transient::GetSingleton().GetData(pc);
		if (transient) {
			return transient->dragon_was_eaten;
		}
		return false;
	}

	void CallDevourmentCompatibility(StaticFunctionTag*, Actor* Pred, Actor* Prey, bool Digested) {
		Devourment_Compatibility(Pred, Prey, Digested);
	}

	bool GetDevourmentCompatibility(StaticFunctionTag*) {
		return Config::GetGeneral().bDevourmentCompat;
	}


	bool DragonCheck(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return false;
		}
		return IsDragon(actor);
	}

	bool IsJumping(StaticFunctionTag*, Actor* actor) {
		return GTS::IsJumping(actor);
	}

	bool IsInAir(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return false;
		}
		return actor->IsInMidair();
	}

	float GetTremorScale(StaticFunctionTag*) {
		return Persistent::GetSingleton().tremor_scale;
	}

	void SetTremorScale(StaticFunctionTag*, float value) {
		//UNUSED REMOVE ME
		Persistent::GetSingleton().tremor_scale = value;
	}

	float GetTremorScaleNPC(StaticFunctionTag*) {
		// UNUSED REMOVE ME
		return Persistent::GetSingleton().npc_tremor_scale;
	}

	void SetTremorScaleNPC(StaticFunctionTag*, float value) {
		Persistent::GetSingleton().npc_tremor_scale = value;
	}

	float GetExperimentFloat(StaticFunctionTag*) {
		return GtsManager::GetSingleton().experiment;
	}

	void SetExperimentFloat(StaticFunctionTag*, float value) {
		GtsManager::GetSingleton().experiment = value;
	}
}

namespace GTS {
	bool register_papyrus_plugin(IVirtualMachine* vm) {
		vm->RegisterFunction("GetDistanceToCamera", PapyrusClass, GetDistanceToCamera);
		vm->RegisterFunction("SetSizeDamageMultiplier", PapyrusClass, SetSizeDamageMultiplier);
		vm->RegisterFunction("SetExperienceMultiplier", PapyrusClass, SetExperienceMultiplier);
		vm->RegisterFunction("SetLegacySounds", PapyrusClass, SetLegacySounds);
		vm->RegisterFunction("GetSizeRelatedDamage", PapyrusClass, GetSizeRelatedDamage);
		vm->RegisterFunction("ModSizeVulnerability", PapyrusClass, ModSizeVulnerability);
		vm->RegisterFunction("GetSizeVulnerability", PapyrusClass, GetSizeVulnerability);
		vm->RegisterFunction("GetStolenAttribute", PapyrusClass, GetStolenAttribute);
		vm->RegisterFunction("GetAttributeBonus", PapyrusClass, GetAttributeBonus);
		vm->RegisterFunction("GetFlatAttributeBonus", PapyrusClass, GetFlatAttributeBonus);
		vm->RegisterFunction("SetGrowthHalfLife", PapyrusClass, SetGrowthHalfLife);
		vm->RegisterFunction("GetGrowthHalfLife", PapyrusClass, GetGrowthHalfLife);
		vm->RegisterFunction("SetAnimSpeed", PapyrusClass, SetAnimSpeed);
		vm->RegisterFunction("SigFig", PapyrusClass, SigFig);
		vm->RegisterFunction("SetPlayerStagger", PapyrusClass, SetPlayerStagger);
		vm->RegisterFunction("SetActorAudioOverride", PapyrusClass, SetActorAudioOverride);
		vm->RegisterFunction("SetNPCProtection", PapyrusClass, SetNPCProtection);
		vm->RegisterFunction("SetPCProtection", PapyrusClass, SetPCProtection);
		vm->RegisterFunction("SetWeightGain", PapyrusClass, SetWeightGain);
		vm->RegisterFunction("DisableCollisionLayerAndMotion", PapyrusClass, DisableCollisionLayerAndMotion);
		vm->RegisterFunction("EnableCollisionLayerAndMotion", PapyrusClass, EnableCollisionLayerAndMotion);
		vm->RegisterFunction("ResetQuestProgression", PapyrusClass, ResetQuestProgression);
		vm->RegisterFunction("Quest_GetProgression", PapyrusClass, Quest_GetProgression);
		vm->RegisterFunction("GetAspectOfGiantessPower", PapyrusClass, GetAspectOfGiantessPower);
		vm->RegisterFunction("AdjustBalanceMode", PapyrusClass, AdjustBalanceMode);
		vm->RegisterFunction("SetProgressionMultiplier", PapyrusClass, SetProgressionMultiplier);
		vm->RegisterFunction("SetStompAi", PapyrusClass, SetStompAi);
		vm->RegisterFunction("SetSandwichAi", PapyrusClass, SetSandwichAi);
		vm->RegisterFunction("SetFollowerInteractions", PapyrusClass, SetFollowerInteractions);
		vm->RegisterFunction("SetVoreAi", PapyrusClass, SetVoreAi);
		vm->RegisterFunction("SetHugsAi", PapyrusClass, SetHugsAi);
		vm->RegisterFunction("SetThighAi", PapyrusClass, SetThighAi);
		vm->RegisterFunction("SetKickAi", PapyrusClass, SetKickAi);
		vm->RegisterFunction("SetButtCrushAi", PapyrusClass, SetButtCrushAi);
		vm->RegisterFunction("SetActorPanic", PapyrusClass, SetActorPanic);
		vm->RegisterFunction("ToggleHostileDamage", PapyrusClass, ToggleHostileDamage);
		vm->RegisterFunction("SetAllowPlayerVore", PapyrusClass, SetAllowPlayerVore);
		vm->RegisterFunction("SetInsectVore", PapyrusClass, SetInsectVore);
		vm->RegisterFunction("SetUndeadVore", PapyrusClass, SetUndeadVore);
		vm->RegisterFunction("DistributeRandomItems", PapyrusClass, DistributeRandomItems);
		vm->RegisterFunction("SetOnlyCombatVore", PapyrusClass, SetOnlyCombatVore);
		vm->RegisterFunction("IncreaseSizeLimit", PapyrusClass, IncreaseSizeLimit);
		vm->RegisterFunction("IncreaseMassLimit", PapyrusClass, IncreaseMassLimit);
		vm->RegisterFunction("DisintegrateTarget", PapyrusClass, DisintegrateTarget);
		vm->RegisterFunction("WasDragonEaten", PapyrusClass, WasDragonEaten);

		vm->RegisterFunction("CallDevourmentCompatibility", PapyrusClass, CallDevourmentCompatibility);
		vm->RegisterFunction("GetDevourmentCompatibility", PapyrusClass, GetDevourmentCompatibility);

		vm->RegisterFunction("DragonCheck", PapyrusClass, DragonCheck);
		vm->RegisterFunction("IsJumping", PapyrusClass, IsJumping);
		vm->RegisterFunction("IsInAir", PapyrusClass, IsInAir);
		vm->RegisterFunction("GetTremorScale", PapyrusClass, GetTremorScale);
		vm->RegisterFunction("SetTremorScale", PapyrusClass, SetTremorScale);
		vm->RegisterFunction("GetTremorScaleNPC", PapyrusClass, GetTremorScaleNPC);
		vm->RegisterFunction("SetTremorScaleNPC", PapyrusClass, SetTremorScaleNPC);
		vm->RegisterFunction("GetExperimentFloat", PapyrusClass, GetExperimentFloat);
		vm->RegisterFunction("SetExperimentFloat", PapyrusClass, SetExperimentFloat);

		return true;
	}
}
