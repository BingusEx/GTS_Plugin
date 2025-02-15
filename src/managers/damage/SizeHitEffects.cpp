#include "managers/animation/Utils/CooldownManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/SizeHitEffects.hpp"
#include "managers/animation/HugShrink.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/animation/Grab.hpp"
#include "managers/CrushManager.hpp"
#include "magic/effects/common.hpp"
#include "utils/actorUtils.hpp"
#include "managers/Rumble.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"

using namespace GTS;




namespace {
	void Prevent_Stagger(Actor* attacker, Actor* receiver) {
		float sizedifference = GetSizeDifference(receiver, attacker, SizeType::GiantessScale, true, true);
		receiver->SetGraphVariableFloat("GiantessScale", sizedifference); // Manages Stagger Resistance inside Behaviors.
		// Prevent stagger anims from playing on GTS, Behaviors read GiantessScale value and disallow stagger if value is > 1.5
	}

	float Hit_CalculateGrowth(float damage, float SizeHunger, float Gigantism) {
		return std::clamp((-damage/2000) * SizeHunger * Gigantism, 0.0f, 0.25f * Gigantism);
	}

	float Hit_CalculateShrink(Actor* attacker, float damage, float SizeHunger, float Gigantism, float resistance) {
		float sizebonus = std::clamp(get_visual_scale(attacker), 0.10f, 1.0f);
		float ShrinkValue = std::clamp(((-damage/600)/SizeHunger/Gigantism * sizebonus) * resistance, 0.0f, 0.25f / Gigantism); // affect limit by decreasing it

		return ShrinkValue;
	}

	bool Hit_ShouldGrow(Actor* receiver) {
		auto& sizemanager = SizeManager::GetSingleton();
		float BalanceMode = sizemanager.BalancedMode();

		bool GrowthEnabled = sizemanager.GetHitGrowth(PlayerCharacter::GetSingleton()) >= 1.0f;
		bool HasPerk = Runtime::HasPerkTeam(receiver, "GrowthOnHitPerk");
		bool Teammate = IsTeammate(receiver) && IsFemale(receiver, true);
		bool IsPlayer = receiver->formID == 0x14;
		
		if (IsPlayer || Teammate) {
			if (GrowthEnabled && HasPerk){
				return true;
			}
		}
		return false;
	}

	bool Hit_ShouldShrink(Actor* receiver) {
		auto& sizemanager = SizeManager::GetSingleton();
		
		bool HasPerk = Runtime::HasPerk(receiver, "GrowthOnHitPerk");
		bool BalanceMode = sizemanager.BalancedMode() >= 2.0f;
		bool IsPlayer = receiver->formID == 0x14;

		if (BalanceMode && receiver->formID == 0x14 && !HasPerk) {
			if (get_target_scale(receiver) > get_natural_scale(receiver, true)) {
				return true;
			}
		}
		return false;
	}

	void TinyAsShield(Actor* receiver, float a_damage) {
		auto grabbedActor = Grab::GetHeldActor(receiver);
		if (grabbedActor) {
			if (IsTeammate(grabbedActor)) {
				return; // Don't kill teammates
			}

			DamageAV(grabbedActor, ActorValue::kHealth, a_damage * 0.50f);
			if (grabbedActor->IsDead() || GetAV(grabbedActor, ActorValue::kHealth) < a_damage * 0.50f) {
				if (!IsBetweenBreasts(grabbedActor)) {
					PrintDeathSource(receiver, grabbedActor, DamageSource::BlockDamage);
				} else {
					PrintDeathSource(receiver, grabbedActor, DamageSource::Breast);
				}

				Grab::DetachActorTask(receiver);
				ModSizeExperience_Crush(receiver, grabbedActor, false);

				auto hand = find_node(receiver, "NPC L Hand [LHnd]");
				if (hand) {
					if (IsLiving(grabbedActor)) {
						SpawnParticle(receiver, 25.0f, "GTS/Damage/Explode.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 5, 4, hand);
						SpawnParticle(receiver, 25.0f, "GTS/Damage/Crush.nif", hand->world.rotate, hand->world.translate, get_visual_scale(grabbedActor) * 5, 4, hand);
					} else {
						SpawnDustParticle(receiver, grabbedActor, "NPC L Hand [LHnd]", 3.0f);
					}
				}
				CrushManager::Crush(receiver, grabbedActor);
				if (!LessGore()) {
					Runtime::PlaySoundAtNode("CrunchImpactSound", receiver, 1.0f, 1.0f, "NPC L Hand [LHnd]");
					Runtime::PlaySoundAtNode("CrunchImpactSound", receiver, 1.0f, 1.0f, "NPC L Hand [LHnd]");
					Runtime::PlaySoundAtNode("CrunchImpactSound", receiver, 1.0f, 1.0f, "NPC L Hand [LHnd]");
				} else {
					Runtime::PlaySoundAtNode("SoftHandAttack", receiver, 1.0f, 1.0f, "NPC L Hand [LHnd]");
				}
				Rumbling::Once("GrabAttackKill", receiver, 8.0f, 0.15f, "NPC L Hand [LHnd]", 0.0f);
				AnimationManager::StartAnim("GrabAbort", receiver); // Abort Grab animation
				Grab::Release(receiver);
			}
		}
	}
	

	void DropTinyChance(Actor* receiver, float damage, float scale) {
		static Timer DropTimer = Timer(0.33); // Check once per .33 sec
		float bonus = 1.0f;
		if (Runtime::HasPerkTeam(receiver, "HugCrush_HugsOfDeath")) {
			return; // Full immunity
		}
		if (Runtime::HasPerkTeam(receiver, "HugCrush_Greed")) {
			bonus = 4.0f; // 4 times bigger damage threshold to cancel hugs
		}
		if (Runtime::HasPerkTeam(receiver, "HugCrush_ToughGrip")) {
			float GetHP = GetHealthPercentage(receiver);
			if (GetHP >= 0.85f) {
				return; // Drop only if hp is < 85%
			}
		}
		if (damage < 6.0f * bonus * scale) {
			return;
		}
		HugShrink::CallRelease(receiver); // Else release
	}

	void DoHitShake(Actor* receiver, float value) {
		if (IsFirstPerson()) {
			value *= 0.05f;
		}
		Rumbling::Once("HitGrowth", receiver, value, 0.15f);
	}

	void HitGrowth(Actor* receiver, Actor* attacker, float GrowthValue, float SizeDifference, float BalanceMode) {
		//log::info("Growth Value: {}", GrowthValue);

		int LaughChance = RandomInt(0, 12);
		int ShrinkChance = RandomInt(0, 5);

		float particlescale = 1.0f;
		float shrinkmult = 1.0f;

		static Timer soundtimer = Timer(1.5);

		float Adjustment = 1.0f * GetSizeFromBoundingBox(attacker);

		bool BlockParticle = IsActionOnCooldown(attacker, CooldownSource::Misc_BeingHit);
		bool LaughBlocked = IsActionOnCooldown(receiver, CooldownSource::Emotion_Laugh);

		DoHitShake(receiver, GrowthValue * 10);
		update_target_scale(receiver, GrowthValue, SizeEffectType::kNeutral);
		
		if (soundtimer.ShouldRunFrame()) {
			Runtime::PlaySoundAtNode("growthSound", receiver, GrowthValue * 2, 1.0f, "NPC Pelvis [Pelv]");
		}
		if (ShrinkChance >= 2) {
			if (get_target_scale(attacker) >= 0.06f/Adjustment) {
				if (SizeDifference >= 2.5f && LaughChance >= 5 && !LaughBlocked) {
					Task_FacialEmotionTask_Smile(receiver, 1.4f, "HitGrowthSmile");
					ApplyActionCooldown(receiver, CooldownSource::Emotion_Laugh);
					PlayLaughSound(receiver, 1.0f, 1);
					particlescale = 2.2f;
					shrinkmult = 6.0f;
				}
			
				update_target_scale(attacker, -GrowthValue/(3.0f * Adjustment*BalanceMode) * shrinkmult, SizeEffectType::kShrink); // Shrink Attacker
				update_target_scale(receiver, GrowthValue/(3.0f * Adjustment*BalanceMode), SizeEffectType::kGrow); // Grow receiver

				if (!BlockParticle) {
					SpawnCustomParticle(attacker, ParticleType::Red, NiPoint3(), "NPC Root [Root]", particlescale);
					ApplyActionCooldown(attacker, CooldownSource::Misc_BeingHit);
				}
			}
		}
	}

	void HitShrink(Actor* receiver, float ShrinkValue) {
		float scale = get_target_scale(receiver);
		float naturalscale = get_natural_scale(receiver, true);

		float lossmod = Runtime::GetFloatOr("SizeLossModifier", 1.0f);
		float modifier = std::clamp(lossmod, 0.10f, 25.0f); // * size loss value

		ShrinkValue *= modifier;

		log::info("Shrink Value: {}", -ShrinkValue);

		if (scale - ShrinkValue < naturalscale) {
			set_target_scale(receiver, naturalscale);
		} else {
			update_target_scale(receiver, -ShrinkValue, SizeEffectType::kShrink);
		}
	}

	void ApplyHitGrowth(Actor* attacker, Actor* receiver, float damage) {
		if (attacker == receiver) {
			return;
		}
		
		auto& sizemanager = SizeManager::GetSingleton();
		float BalanceMode = sizemanager.BalancedMode();

		float SizeHunger = 1.0f + Ench_Hunger_GetPower(receiver);
		float Gigantism = 1.0f + Ench_Aspect_GetPower(receiver);
		
		float SizeDifference = GetSizeDifference(receiver, attacker, SizeType::VisualScale, true, true);
		float resistance = Potion_GetShrinkResistance(receiver);
	
		if (Hit_ShouldGrow(receiver)) { // if has perk. Wins over balance mode if true
			HitGrowth(receiver, attacker, Hit_CalculateGrowth(damage, SizeHunger, Gigantism), SizeDifference, BalanceMode);
		} else if (Hit_ShouldShrink(receiver)) { // else, if balance mode is on, Shrink us
			HitShrink(receiver, Hit_CalculateShrink(attacker, damage, SizeHunger, Gigantism, resistance));
		}
	}

	void ApplyToTinies(Actor* attacker, Actor* receiver, float damage) {
		float sizedifference = GetSizeDifference(receiver, attacker, SizeType::VisualScale, true, true);
		DropTinyChance(receiver, -damage, sizedifference);
		TinyAsShield(receiver, -damage);
	}
}


namespace GTS {

	SizeHitEffects& SizeHitEffects::GetSingleton() noexcept {
		static SizeHitEffects instance;
		return instance;
	}

	std::string SizeHitEffects::DebugName() {
		return "SizeHitEffects";
	}

	void SizeHitEffects::ApplyEverything(Actor* attacker, Actor* receiver, float damage) {
		ApplyHitGrowth(attacker, receiver, damage);
		ApplyToTinies(attacker, receiver, damage);
		Prevent_Stagger(attacker, receiver);
	}

	void SizeHitEffects::PerformInjuryDebuff(Actor* giant, Actor* tiny, float damage, int random) { // Used as a debuff
		if (!tiny->IsDead() && Runtime::HasPerkTeam(giant, "RavagingInjuries")) {
			if (random > 0) {

				if (Runtime::HasPerkTeam(giant, "DevastatingSprint") && giant->AsActorState()->IsSprinting() && !IsGtsBusy(giant)) {
					damage *= 3.0f;
					random = 1; // always apply
				}

				int rng = (RandomInt(0, random));
				if (rng <= 2) {
					float sizediff = GetSizeDifference(giant, tiny, SizeType::VisualScale, true, true);
					if (sizediff < 3.0f) {
						return;
					}

					if (!IsLiving(tiny)) {
						SpawnDustParticle(giant, tiny, "NPC Root [Root]", 1.0f);
					} else {
						auto root = find_node(tiny, "NPC Root [Root]");
						if (root) {
							SpawnParticle(tiny, 0.20f, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, get_visual_scale(tiny), 7, root);
						}
					}
					if (damage > 0) {
						InflictSizeDamage(giant, tiny, damage * 1.5f);
					}
				}
			}
		}
	}
}
