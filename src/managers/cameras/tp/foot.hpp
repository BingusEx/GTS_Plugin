#pragma once
#include "managers/cameras/tpState.hpp"
#include "spring.hpp"

using namespace RE;

namespace Gts {
	class Foot : public ThirdPersonCameraState {
		public:
			virtual void EnterState() override;

			virtual NiPoint3 GetPlayerLocalOffset(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetPlayerLocalOffsetProne(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetOffset(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetCombatOffset(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetOffsetProne(const NiPoint3& cameraPos) override;

			virtual NiPoint3 GetCombatOffsetProne(const NiPoint3& cameraPos) override;

			//virtual BoneTarget GetBoneTarget() override;

		protected:
			virtual NiPoint3 GetFootPos();

			Spring3 smoothFootPos = Spring3(NiPoint3(0.0f, 0.0f, 0.0f), 0.5f);

			Spring smoothScale = Spring(0.0f, 0.f);
	};
}
