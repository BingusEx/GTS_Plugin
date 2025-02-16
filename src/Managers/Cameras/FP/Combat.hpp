#pragma once

#include "Managers/Cameras/FPState.hpp"

namespace GTS {
	class FirstPersonCombat : public FirstPersonCameraState {
		public:
			virtual float GetScaleOverride(bool IsCrawling) override;
	};
}
