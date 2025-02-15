#include "managers/cameras/fp/combat.hpp"
#include "utils/actorUtils.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "scale/height.hpp"



namespace {
	const float BASE_OVERRIDE = 1.0f;
}

namespace GTS {
	float FirstPersonCombat::GetScaleOverride(bool IsCrawling) {
		float proneFactor = 1.0f;
		/*if (IsCrawling) {
		        proneFactor = GetProneAdjustment();
		   }*/
		//return BASE_OVERRIDE * proneFactor;
		return proneFactor;
	}
}
