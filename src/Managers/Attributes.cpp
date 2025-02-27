#include "Managers/Attributes.hpp"

#include "Config/Config.hpp"

#include "Managers/Damage/TinyCalamity.hpp"
#include "Managers/GtsSizeManager.hpp"

using namespace REL;
using namespace GTS;

// TODO move away from polling

namespace {

	void SetINIFloat(std::string_view name, float value) {
		auto ini_conf = GameSettingCollection::GetSingleton();
		std::string s_name(name);
		Setting* setting = ini_conf->GetSetting(s_name.c_str());
		if (setting) {
			setting->data.f=value; // If float
			ini_conf->WriteSetting(setting);
		}
	}

	float GetMovementSlowdown(Actor* tiny) {
		auto transient = Transient::GetSingleton().GetData(tiny);
		if (transient) {
			return transient->MovementSlowdown;
		}
		return 1.0f;
	}


	void ManagePerkBonuses(Actor* actor) {
		auto& SizeManager = SizeManager::GetSingleton();
		float BalanceModeDiv = SizeManager::BalancedMode() ? 2.0f : 1.0f;
		float gigantism = 1.0f + (Ench_Aspect_GetPower(actor) * 0.30f);

		float BaseGlobalDamage = SizeManager::GetSingleton().GetSizeAttribute(actor, SizeAttribute::Normal);
		float BaseSprintDamage = SizeManager::GetSingleton().GetSizeAttribute(actor, SizeAttribute::Sprint);
		float BaseFallDamage = SizeManager::GetSingleton().GetSizeAttribute(actor, SizeAttribute::JumpFall);

		float ExpectedGlobalDamage = 1.0f;
		float ExpectedSprintDamage = 1.0f;
		float ExpectedFallDamage = 1.0f;

		///Normal Damage
		if (Runtime::HasPerkTeam(actor, "Cruelty")) {
			ExpectedGlobalDamage += 0.15f/BalanceModeDiv;
		}
		if (Runtime::HasPerkTeam(actor, "RealCruelty")) {
			ExpectedGlobalDamage += 0.35f/BalanceModeDiv;
		}
		if (IsGrowthSpurtActive(actor)) {
			ExpectedGlobalDamage *= (1.0f + (0.35f/BalanceModeDiv));
		}
		if (Runtime::HasPerkTeam(actor, "MightOfGiants")) {
			ExpectedGlobalDamage *= 1.15f; // +15% damage
		}

		///Sprint Damage
		if (Runtime::HasPerkTeam(actor, "QuickApproach")) {
			ExpectedSprintDamage += 0.25f/BalanceModeDiv;
		}
		///Fall Damage
		if (Runtime::HasPerkTeam(actor, "MightyLegs")) {
			ExpectedFallDamage += 0.3f/BalanceModeDiv;
		}
		///Buff by enchantment
		ExpectedGlobalDamage *= gigantism;
		ExpectedSprintDamage *= gigantism;
		ExpectedFallDamage *= gigantism;

		if (BaseGlobalDamage != ExpectedGlobalDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedGlobalDamage, SizeAttribute::Normal);
		}
		if (BaseSprintDamage != ExpectedSprintDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedSprintDamage, SizeAttribute::Sprint);
		}
		if (BaseFallDamage != ExpectedFallDamage) {
			SizeManager.SetSizeAttribute(actor, ExpectedFallDamage, SizeAttribute::JumpFall);
		}
	}

	// Todo unify the functions
	void UpdateActors(Actor* actor) {
		if (actor) {
			ManagePerkBonuses(actor);
			if (actor->formID == 0x14) {
				TinyCalamity_BonusSpeed(actor); // Manages SMT bonuses
			}
		}
	}
}


namespace GTS {
	AttributeManager& AttributeManager::GetSingleton() noexcept {
		static AttributeManager instance;
		return instance;
	}

	std::string AttributeManager::DebugName() {
		return "AttributeManager";
	}

	void AttributeManager::Update() {
		static Timer timer = Timer(0.5);

		if (timer.ShouldRunFrame()) { // Run once per 0.5 sec
			for (auto actor: find_actors()) {
				if (actor) {
					if (actor->Is3DLoaded()) {
						UpdateActors(actor);
					}
				}
			}
		}
	}


	void AttributeManager::OverrideSMTBonus(float Value) {
		auto ActorAttributes = Persistent::GetSingleton().GetData(PlayerCharacter::GetSingleton());
		if (ActorAttributes) {
			ActorAttributes->smt_run_speed = Value;
		}
	}

	float AttributeManager::GetAttributeBonus(Actor* actor, ActorValue av) const {
		auto profiler = Profilers::Profile("Attributes: GetAttributeBonus");
		if (!actor) {
			return 1.0f;
		}

		float BalancedMode = SizeManager::BalancedMode() ? 2.0f : 1.0f;
		float natural_scale = get_natural_scale(actor, true);
		float scale = get_giantess_scale(actor);
		if (scale <= 0) {
			scale = 1.0f;
		} 
		if (scale < 1.0f) {
			scale /= natural_scale; 
			// Fix: negative bonuses when natural scale is < 1.0
			// No Fix: 0.91/1.0 = 0.91   (0.91 is just example of current size)
			// Fix:    0.91/0.91(natural size) = 1.0
		}
		switch (av) {

			case ActorValue::kHealth: {
				float might = 1.0f + Potion_GetMightBonus(actor);

				if (actor->formID == 0x14) {
					if (HasSMT(actor)) {
						scale += 1.0f;
					}
					if (actor->AsActorState()->IsSprinting() && Runtime::HasPerk(actor, "QuickApproach")) {
						scale *= 1.30f;
					}
				}

				scale *= might;
				float resistance = std::clamp(1.0f / scale, 0.001f, 3.0f); // 0.001% as max resistance, -300% is a max vulnerability.
				return resistance;

			}

			case ActorValue::kCarryWeight: {
				const float BonusCarryMult = Config::GetBalance().fStatBonusCarryWeightMult;
				float power = (BonusCarryMult/BalancedMode);

				float might = 1.0f + Potion_GetMightBonus(actor);

				if (actor->formID == 0x14 && HasSMT(actor)) {
					scale += 3.0f;
				}
				if (scale > 1.0f) {
					return (power*scale*might) + 1.0f - power;
				} else {
					return 1.0f * might; // Don't reduce it if scale is < 1.0
				}
			}

			case ActorValue::kSpeedMult: {

				constexpr SoftPotential MS_adjustment{ 
					.k = 0.132f, //0.132
					.n = 0.86f,  //0.86
					.s = 1.12f,  //1.12
					.o = 1.0f,
					.a = 0.0f, //Default is 0
				};

				float MS_mult = soft_core(scale, MS_adjustment);
				float MS_mult_limit = std::clamp(MS_mult, 0.750f, 1.0f);
				float Multy = std::clamp(MS_mult, 0.70f, 1.0f);
				float speed_mult_walk = soft_core(scale, this->speed_adjustment_walk);
				float bonusspeed = std::clamp(speed_mult_walk, 0.90f, 1.0f);
				
				auto actorData = Persistent::GetSingleton().GetData(actor);
				float Bonus = 1.0f;

				float Slowdown = GetMovementSlowdown(actor);

				if (actorData) {
					Bonus = actorData->smt_run_speed;
				}

				float power = 1.0f * Slowdown * (Bonus/2.0f + 1.0f)/MS_mult/MS_mult_limit/Multy/bonusspeed;
				if (scale > 1.0f) {
					return power;
				}
				else {
					return scale * Slowdown * (Bonus/2.0f + 1.0f);
				}
			}

			case ActorValue::kAttackDamageMult: {
				if (actor->formID == 0x14 && HasSMT(actor)) {
					scale += 1.0f;
				}
				const float BonusDamageMult = Config::GetBalance().fStatBonusDamageMult;
				const float DamageStorage = 1.0f + ((BonusDamageMult) * scale - 1.0f);

				float might = 1.0f + Potion_GetMightBonus(actor);

				if (scale > 1.0f) {
					return DamageStorage * might;
				} else {
					return scale * might;
				}
			}

			case ActorValue::kJumpingBonus: { // Used through MCM only (display bonus jump height)
				float power = 1.0f;
				float defaultjump = 1.0f + (1.0f * (scale - 1) * power);
				float might = 1.0f + Potion_GetMightBonus(actor);
				if (scale > 1.0f) {
					return defaultjump * might;
				} else {
					return scale * might;
				}
			}
			default: {
				return 1.0f;
			}
		}
	}

	float AttributeManager::AlterGetAv(Actor* actor, ActorValue av, float originalValue) {
		float bonus = 1.0f;

		auto& attributes = AttributeManager::GetSingleton();
		switch (av) {
			case ActorValue::kCarryWeight: {
				bonus = attributes.GetAttributeBonus(actor, av);
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient != nullptr) {
					transient->carryweight_boost = (originalValue * bonus) - originalValue;
				}
				break;
			}
		}

		return originalValue * bonus;
	}

	float AttributeManager::AlterGetBaseAv(Actor* actor, ActorValue av, float originalValue) {
		float finalValue = originalValue;

		switch (av) {
			case ActorValue::kHealth: { // 27.03.2024: Health boost is still applied, but for Player only and only if having matching perks
				float bonus = 1.0f;
				auto& attributes = AttributeManager::GetSingleton();
				float scale = get_giantess_scale(actor);
				if (scale <= 0) {
					scale = 1.0f;
				}

				if (scale > 1.0f) {
					bonus = attributes.GetAttributeBonus(actor, av);
				} else {
					//Linearly decrease such that:
					//at zero scale health=0.0
					bonus = scale;
				}
				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kHealth); // calc health from the perk bonuses
				finalValue = originalValue + perkbonus; // add flat health on top
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient) {
					transient->health_boost = finalValue - originalValue;
				}
				return finalValue;
			}
			case ActorValue::kMagicka: {
				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kMagicka);
				finalValue = originalValue + perkbonus;
				return finalValue;
			}
			case ActorValue::kStamina: {
				float perkbonus = GetStolenAttributes_Values(actor, ActorValue::kStamina);
				finalValue = originalValue + perkbonus;
				return finalValue;
			}
		}
		return finalValue;
	}

	float AttributeManager::AlterSetBaseAv(Actor* actor, ActorValue av, float originalValue) {
		float finalValue = originalValue;

		switch (av) {
			case ActorValue::kHealth: {
				auto transient = Transient::GetSingleton().GetData(actor);
				if (transient) {
					float lastEdit = transient->health_boost;
					if (finalValue - lastEdit > 0.0f) {
						finalValue -= lastEdit;
					}
				}
			}
		}

		return finalValue;
	}

	float AttributeManager::AlterGetPermenantAv(Actor* actor, ActorValue av, float originalValue) {
		return originalValue;
	}

	float AttributeManager::AlterGetAvMod(float originalValue, Actor* actor, ACTOR_VALUE_MODIFIER a_modifier, ActorValue a_value) {
		return originalValue;
	}

	float AttributeManager::AlterMovementSpeed(Actor* actor, const NiPoint3& direction) {
		float bonus = 1.0f;
		if (actor) {
			auto& attributes = AttributeManager::GetSingleton();
			bonus = attributes.GetAttributeBonus(actor, ActorValue::kSpeedMult);
		}
		return bonus;
	}
}
