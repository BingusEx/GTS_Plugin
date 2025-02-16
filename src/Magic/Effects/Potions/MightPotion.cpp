#include "Magic/Effects/Potions/MightPotion.hpp"
#include "Magic/Effects/Common.hpp"

namespace GTS {

	std::string MightPotion::GetName() {
		return "MightPotion";
	}

	MightPotion::MightPotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("EffectMightPotionExtreme")) {
			this->Power = 0.45f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectMightPotionStrong")) {
			this->Power = 0.30f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectMightPotionNormal")) {
			this->Power = 0.20f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectMightPotionWeak")) {
			this->Power = 0.10f;
		} else if (base_spell == Runtime::GetMagicEffect("AlchEffectMight")) {
			RecordPotionMagnitude(GetActiveEffect(), this->Power, 0.45f);
		}
	}


	void MightPotion::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetMightBonus(caster, this->Power, true); // Disallow potions to stack
			Potion_Penalty(caster);
		}
	}

	void MightPotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			Potion_SetMightBonus(caster, -this->Power, true);
		}
	}
}
