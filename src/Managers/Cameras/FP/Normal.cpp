#include "Managers/Cameras/FP/Normal.hpp"

namespace GTS {
	float FirstPerson::GetScaleOverride(bool IsCrawling) {
		constexpr float proneFactor = 1.005f;
		if (IsCrawling) {
			return proneFactor; // 1.0 only if we crawl
		} else {
			return -1.0f;
		}
	}
}
