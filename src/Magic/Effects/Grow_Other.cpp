
#include "Magic/Effects/Grow_Other.hpp"
#include "Magic/Effects/Common.hpp"

namespace GTS {

	std::string GrowOther::GetName() {
		return "GrowOther";
	}

	void GrowOther::OnUpdate() {

		constexpr float BASE_POWER = 0.00045f;
		constexpr float BONUS = 0.00045f;
		constexpr float SMT_BONUS = 2.0f;
		constexpr float DUAL_CAST_BONUS = 2.0f;
		constexpr float CRUSH_GROWTH_CHECK = 1.4f;

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

		if (base_spell == Runtime::GetMagicEffect("GrowAllyAdept")) {
			power *= 1.32f;
		} else if (base_spell == Runtime::GetMagicEffect("GrowAllyExpert")) {
			power *= 1.75f;
		}

		float caster_scale = get_visual_scale(caster);
		float target_scale = get_visual_scale(target);

		if (Runtime::GetFloat("CrushGrowthRate") >= CRUSH_GROWTH_CHECK) {
			power += BONUS;
		}

		if (IsDualCasting()) {
			power *= DUAL_CAST_BONUS;
		}
		if (HasSMT(caster)) {
			power *= SMT_BONUS;
		}
		float Gain = power * ((caster_scale * 0.50f + 0.50f) * target_scale);
		float Lose = power * (target_scale * 0.25f);

		Grow_Ally(caster, target, Gain, Lose);
	}
}
