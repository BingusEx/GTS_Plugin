#pragma once

#include "Managers/Cameras/TPState.hpp"

namespace GTS {

	class Foot : public ThirdPersonCameraState {
		public:
			virtual void EnterState() override;

			virtual NiPoint3 GetPlayerLocalOffset(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetPlayerLocalOffsetProne(const NiPoint3& cameraPos) override;
		protected:
			virtual NiPoint3 GetFootPos();

			Spring3 smoothFootPos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

			Spring smoothScale = Spring(1.0f, 0.5f);
	};
}
