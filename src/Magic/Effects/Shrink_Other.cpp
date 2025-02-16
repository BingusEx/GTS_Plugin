
#include "Magic/Effects/Common.hpp"
#include "Magic/Effects/Shrink_Other.hpp"

namespace GTS {

	std::string ShrinkOther::GetName() {
		return "ShrinkOther";
	}

	void ShrinkOther::OnUpdate() {

		constexpr float BASE_POWER = 0.00180f;
		constexpr float CRUSH_BONUS = 0.00180f;
		constexpr float GROWTH_AMOUNT_BONUS = 1.4f;
		constexpr float DUAL_CAST_BONUS = 2.0f;
		constexpr float SCALE_FACTOR = 0.5f;

		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		float power = BASE_POWER;

		auto base_spell = GetBaseEffect();

		if (base_spell == Runtime::GetMagicEffect("ShrinkAllyAdept")) {
			power *= 1.33f;
		} else if (base_spell == Runtime::GetMagicEffect("ShrinkAllyExpert")) {
			power *= 1.75f;
		}

		float caster_scale = get_visual_scale(caster);
		float target_scale = get_visual_scale(target);

		if (Runtime::GetFloat("CrushGrowthRate") >= GROWTH_AMOUNT_BONUS) {
			power += CRUSH_BONUS;
		}

		if (IsDualCasting()) {
			power *= DUAL_CAST_BONUS;
		}

		if (target_scale > Minimum_Actor_Scale) {
			if (!IsHostile(target, caster)) {
				ShrinkActor(target, power*0.10f, 0);
			}
		} else {
			set_target_scale(target, Minimum_Actor_Scale);
		}
	}
}
