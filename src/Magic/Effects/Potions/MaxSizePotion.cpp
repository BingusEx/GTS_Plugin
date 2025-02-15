#include "magic/effects/Potions/MaxSizePotion.hpp"
#include "managers/GtsSizeManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/GtsManager.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "magic/magic.hpp"
#include "scale/scale.hpp"

// A potion that TEMPORARILY increases max possible size

namespace {
	void TempBonusMaxSize_Modify(Actor* giant, float value) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			saved_data->potion_max_size += value;
		}
	}
}


namespace GTS {
	std::string MaxSizePotion::GetName() {
		return "MaxSizePotion";
	}


	MaxSizePotion::MaxSizePotion(ActiveEffect* effect) : Magic(effect) {

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("EffectSizePotionExtreme")) {
			this->Power = 0.35f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectSizePotionStrong")) {
			this->Power = 0.20f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectSizePotionNormal")) {
			this->Power = 0.15f;
		} else if (base_spell == Runtime::GetMagicEffect("EffectSizePotionWeak")) {
			this->Power = 0.10f;
		} else if (base_spell == Runtime::GetMagicEffect("AlchEffectSize")) {
			RecordPotionMagnitude(GetActiveEffect(), this->Power, 0.35f);
		}
	}


	void MaxSizePotion::OnStart() {
		auto caster = GetCaster();
		if (caster) {
			TempBonusMaxSize_Modify(caster, this->Power);
			Potion_Penalty(caster);
		}
	}

	void MaxSizePotion::OnFinish() {
		auto caster = GetCaster();
		if (caster) {
			TempBonusMaxSize_Modify(caster, -this->Power);
		}
	}
}
