#pragma once
#include "managers/cameras/fpState.hpp"



namespace GTS {
	class FirstPersonCombat : public FirstPersonCameraState {
		public:
			virtual float GetScaleOverride(bool IsCrawling) override;
	};
}
