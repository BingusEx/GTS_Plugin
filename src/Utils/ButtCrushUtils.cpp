#include "Utils/ButtCrushUtils.hpp"

namespace GTS {

	float GetButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float butt_crush_size = std::clamp(saved_data->buttcrush_max_size, 0.0f, 1000000.0f);
			return butt_crush_size;
		}
		return 0.0f;
	}

	float GetGrowthCount(Actor* giant) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			return transient->ButtCrushGrowthAmount;
		}
		return 1.0f;
	}

	float GetGrowthLimit(Actor* actor) {
		float limit = 0;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_GrowingDisaster")) {
			limit += 3.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			limit += 4.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_LoomingDoom")) {
			limit += 5.0f;
		}
		return limit;
	}

	float GetButtCrushDamage(Actor* actor) {
		float damage = 1.0f;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_KillerBooty")) {
			damage += 0.30f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			damage += 0.70f;
		}
		return damage;
	}

	void ModGrowthCount(Actor* giant, float value, bool reset) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			transient->ButtCrushGrowthAmount += value;
			if (reset) {
				transient->ButtCrushGrowthAmount = 0.0f;
			}
		}
	}

	void RecordStartButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			saved_data->buttcrush_start_scale = get_target_scale(giant);
		}
	}

	void SetButtCrushSize(Actor* giant, float value, bool reset) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float scale = game_getactorscale(giant);
			saved_data->buttcrush_max_size += value * scale;

			if (!reset && GetGrowthCount(giant) <= 1) {
				RecordStartButtCrushSize(giant);
			}

			if (reset) {
				float remove_size = saved_data->buttcrush_max_size;
				float start_scale = saved_data->buttcrush_start_scale;

				start_scale = std::max(start_scale, get_natural_scale(giant));

				set_target_scale(giant, start_scale);

				saved_data->buttcrush_max_size = 0;
				saved_data->buttcrush_start_scale = 0.0f;
			}
		}
	}
}