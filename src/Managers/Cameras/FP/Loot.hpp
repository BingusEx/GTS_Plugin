#pragma once

#include "Managers/Cameras/FPState.hpp"

namespace GTS {
	class FirstPersonLoot : public FirstPersonCameraState {
		public:
			virtual float GetScaleOverride(bool IsCrawling) override;
	};
}
