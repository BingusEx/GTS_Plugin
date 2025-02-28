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

	//Unused
	float GetDistanceToCamera(StaticFunctionTag*, Actor* actor) {
		return get_distance_to_camera(actor);
	}

	//Unused
	float GetSizeRelatedDamage(StaticFunctionTag*, Actor* actor, float attribute) {
		SizeAttribute Attribute = static_cast<SizeAttribute>(attribute);
		return SizeManager::GetSizeAttribute(actor, Attribute);
	}

	//Unused
	float GetSizeVulnerability(StaticFunctionTag*, Actor* actor) {
		return SizeManager::GetSingleton().GetSizeVulnerability(actor);
	}

	//Unused TODO
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

	//Unused <- Ported
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

	//Unused <- Ported
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

	//Unused
	bool ModSizeVulnerability(StaticFunctionTag*, Actor* actor, float amt) {
		if (actor) {
			SizeManager::GetSingleton().ModSizeVulnerability(actor, amt);
			return true;
		}
		return false;
	}

	//Unused
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

	//Unused
	float GetGrowthHalfLife(StaticFunctionTag*, Actor* actor) {
		if (actor) {
			auto actor_data = Persistent::GetSingleton().GetData(actor);
			if (actor_data) {
				return actor_data->half_life;
			}
		}
		return 0.05f;
	}

	//Unused But keep it
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

	//Unused
	// From https://stackoverflow.com/questions/17211122/formatting-n-significant-digits-in-c-without-scientific-notation
	std::string format(double f, int n) {
		if (f == 0) {
			return "0";
		}

		int d = static_cast<int>(ceil(::log10(f < 0 ? -f : f))); /*digits before decimal point*/
		double order = ::pow(10., n - d);
		std::stringstream ss;
		ss << std::fixed << std::setprecision(std::max(n - d, 0)) << round(f * order) / order;
		return ss.str();
	}

	//Unused
	std::string SigFig(StaticFunctionTag*, float number, int sf) {
		return format(number, sf);
	}

	//Unused
	void EnableCollisionLayerAndMotion(StaticFunctionTag*, TESObjectREFR* ref) {
		if (!ref) {
			return;
		}
		auto current3D = ref->GetCurrent3D();
		if (!current3D) {
			return; // Retry next frame
		}
		current3D->SetMotionType(static_cast<uint32_t>(hkpMotion::MotionType::kCharacter), true, true, true);
		current3D->SetCollisionLayer(COL_LAYER::kCharController);
	}

	//Unused
	void DisableCollisionLayerAndMotion(StaticFunctionTag*, TESObjectREFR* ref) {
		if (!ref) {
			return;
		}
		auto current3D = ref->GetCurrent3D();
		if (!current3D) {
			return; // Retry next frame
		}
		current3D->SetMotionType(static_cast<uint32_t>(hkpMotion::MotionType::kKeyframed), true, true, true);
		current3D->SetCollisionLayer(COL_LAYER::kNonCollidable);
	}

	//Keep
	void ResetQuestProgression(StaticFunctionTag*) {
		ResetQuest();
	}

	//Keep
	float Quest_GetProgression(StaticFunctionTag*, int stage) {
		return GetQuestProgression(stage);
	}

	//UNUSED
	float GetAspectOfGiantessPower(StaticFunctionTag*) {
		auto player = PlayerCharacter::GetSingleton();
		return Ench_Aspect_GetPower(player);
	}

	//UNUSED
	void DistributeRandomItems(StaticFunctionTag*) {
		//DistributeChestItems();
	}

	//UNUSED
	void IncreaseSizeLimit(StaticFunctionTag*, float value, Actor* caster) {
		AdjustSizeLimit(value, caster);
	}

	//UNUSED
	void IncreaseMassLimit(StaticFunctionTag*, float value, Actor* caster) {
		AdjustMassLimit(value, caster);
	}

	//UNUSED
	void DisintegrateTarget(StaticFunctionTag*, Actor* actor) {
		if (actor) {
			Disintegrate(actor);
		}
	}

	//Keep
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

	//Not really needed. Calldevourmentcompat could just do this check itself
	bool GetDevourmentCompatibility(StaticFunctionTag*) {
		return Config::GetGeneral().bDevourmentCompat;
	}


	//Called in unused psc function. Remove
	bool DragonCheck(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return false;
		}
		return IsDragon(actor);
	}

	//UNUSED
	bool IsInAir(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return false;
		}
		return actor->IsInMidair();
	}

	//UNUSED
	float GetExperimentFloat(StaticFunctionTag*) {
		return GtsManager::GetSingleton().experiment;
	}

	//UNUSED
	void SetExperimentFloat(StaticFunctionTag*, float value) {
		GtsManager::GetSingleton().experiment = value;
	}
}

namespace GTS {
	bool register_papyrus_plugin(IVirtualMachine* vm) {
		vm->RegisterFunction("GetDistanceToCamera", PapyrusClass, GetDistanceToCamera);
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
		vm->RegisterFunction("DisableCollisionLayerAndMotion", PapyrusClass, DisableCollisionLayerAndMotion);
		vm->RegisterFunction("EnableCollisionLayerAndMotion", PapyrusClass, EnableCollisionLayerAndMotion);
		vm->RegisterFunction("ResetQuestProgression", PapyrusClass, ResetQuestProgression);
		vm->RegisterFunction("Quest_GetProgression", PapyrusClass, Quest_GetProgression);
		vm->RegisterFunction("GetAspectOfGiantessPower", PapyrusClass, GetAspectOfGiantessPower);
		vm->RegisterFunction("DistributeRandomItems", PapyrusClass, DistributeRandomItems);
		vm->RegisterFunction("IncreaseSizeLimit", PapyrusClass, IncreaseSizeLimit);
		vm->RegisterFunction("IncreaseMassLimit", PapyrusClass, IncreaseMassLimit);
		vm->RegisterFunction("DisintegrateTarget", PapyrusClass, DisintegrateTarget);
		vm->RegisterFunction("WasDragonEaten", PapyrusClass, WasDragonEaten);

		vm->RegisterFunction("CallDevourmentCompatibility", PapyrusClass, CallDevourmentCompatibility);
		vm->RegisterFunction("GetDevourmentCompatibility", PapyrusClass, GetDevourmentCompatibility);

		vm->RegisterFunction("DragonCheck", PapyrusClass, DragonCheck);
		vm->RegisterFunction("IsJumping", PapyrusClass, IsJumping);
		vm->RegisterFunction("IsInAir", PapyrusClass, IsInAir);
		vm->RegisterFunction("GetExperimentFloat", PapyrusClass, GetExperimentFloat);
		vm->RegisterFunction("SetExperimentFloat", PapyrusClass, SetExperimentFloat);

		return true;
	}
}
