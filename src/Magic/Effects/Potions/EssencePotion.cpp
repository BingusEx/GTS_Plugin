#include "managers/animation/Utils/CooldownManager.hpp"
#include "magic/effects/Potions/EssencePotion.hpp"
#include "magic/effects/common.hpp"
#include "Constants.hpp"
#include "data/runtime.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"

using namespace GTS;

namespace {

    void shake_screen_do_moan(Actor* giant, float power) {
		if (power >= 0.07f) {
			bool Blocked = IsActionOnCooldown(giant, CooldownSource::Emotion_Moan);
			if (!Blocked) {
				PlayMoanSound(giant, 1.0f);
				ApplyActionCooldown(giant, CooldownSource::Emotion_Moan);
			}
		}

		if (giant->formID == 0x14) {
			float shake = 12.0f * power;
			shake_camera(giant, 0.50f * shake, 0.38f * shake);
		}
    }

	void PermBonusMaxSize_Modify(Actor* caster, float power) {
		if (caster && caster->formID == 0x14) {
			float scale = get_visual_scale(caster);

			TESGlobal* BonusSize = Runtime::GetGlobal("ExtraPotionSize"); 
			// Bonus size is added on top of all size calculations through this global
			// Applied inside GtsManager.cpp (script)
			if (BonusSize) {
				BonusSize->value += power/Characters_AssumedCharSize; // convert to m
			}

			SpawnCustomParticle(caster, ParticleType::Red, NiPoint3(), "NPC COM [COM ]", scale * ((power < 0.10 ? power : 0.08f) * 25)); // Just some nice visuals
			shake_screen_do_moan(caster, power < 0.10 ? power : 0.08f);
		}
	}
}

namespace GTS {
	std::string EssencePotion::GetName() {
		return "EssencePotion";
	}

    EssencePotion::EssencePotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionWeak")) {
			this->power = 0.02f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionNormal")) {
			this->power = 0.04f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionStrong")) {
			this->power = 0.06f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectEssencePotionExtreme")) {
			this->power = 0.08f; 
		} else if (base_spell == Runtime::GetMagicEffect("AlchEffectEssence")) {
			RecordPotionMagnitude(GetActiveEffect(), this->power, 0.08f);
		}
	}

	void EssencePotion::OnStart() {
		auto caster = GetCaster();

		if (caster) { // player exclusive
			PermBonusMaxSize_Modify(caster, this->power);
			Potion_Penalty(caster);
        }
	}
}