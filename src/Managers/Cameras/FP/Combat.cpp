#include "Managers/Cameras/FP/Combat.hpp"

namespace GTS {

	float FirstPersonCombat::GetScaleOverride(bool IsCrawling) {
		constexpr float proneFactor = 1.0f;
		return proneFactor;
	}
}
