#include "Managers/Cameras/FP/Combat.hpp"

namespace {
	constexpr float BASE_OVERRIDE = 1.0f;
}

namespace GTS {

	float FirstPersonCombat::GetScaleOverride(bool IsCrawling) {
		constexpr float proneFactor = 1.0f;
		/*if (IsCrawling) {
		        proneFactor = GetProneAdjustment();
		   }*/
		//return BASE_OVERRIDE * proneFactor;
		return proneFactor;
	}
}
