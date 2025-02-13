#pragma once
#include "managers/cameras/fpState.hpp"



namespace Gts {
	class FirstPerson : public FirstPersonCameraState {
		public:
			virtual float GetScaleOverride(bool IsCrawling) override;
	};
}
