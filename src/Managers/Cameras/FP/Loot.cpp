#include "Managers/Cameras/FP/Loot.hpp"


namespace {
	constexpr float BASE_OVERRIDE = 0.7f;
}

namespace GTS {
	float FirstPersonLoot::GetScaleOverride(bool IsCrawling) {
		float proneFactor = 1.0f;
		if (IsCrawling) {
			proneFactor = GetProneAdjustment();
		}
		return BASE_OVERRIDE * proneFactor;
	}
}
