#include "Managers/Cameras/State.hpp"
#include "Managers/Cameras/CamUtil.hpp"

namespace GTS {

	void CameraState::EnterState() {}
	void CameraState::ExitState() {}

	float CameraState::GetScale() {
		auto player = GetCameraActor();
		float result = get_visual_scale(player);
		if (!player) {
			return 1.0f;
		}
		return result;
	}

	float CameraState::GetScaleOverride(bool IsCrawling) {
		return -1.0f;
	}

	NiPoint3 CameraState::GetOffset(const NiPoint3& cameraPosLocal) {
		return NiPoint3(0.0f, 0.0f, 0.0f);
	}

	NiPoint3 CameraState::GetOffsetProne(const NiPoint3& cameraPosLocal) {
		return this->GetOffset(cameraPosLocal);
	}

	NiPoint3 CameraState::GetOffset(const NiPoint3& cameraPosLocal, bool IsCrawling) {
		if (IsCrawling) {
			return this->GetOffsetProne(cameraPosLocal);
		} else {
			return this->GetOffset(cameraPosLocal);
		}
	}

	NiPoint3 CameraState::GetCombatOffset(const NiPoint3& cameraPosLocal) {
		return NiPoint3(0.0f, 0.0f, 0.0f);
	}

	NiPoint3 CameraState::GetCombatOffsetProne(const NiPoint3& cameraPosLocal) {
		return this->GetCombatOffset(cameraPosLocal);
	}

	NiPoint3 CameraState::GetCombatOffset(const NiPoint3& cameraPosLocal, bool IsCrawling) {
		if (IsCrawling) {
			return this->GetCombatOffsetProne(cameraPosLocal);
		} else {
			return this->GetCombatOffset(cameraPosLocal);
		}
	}

	NiPoint3 CameraState::GetPlayerLocalOffset(const NiPoint3& cameraPosLocal) {
		return NiPoint3(0.0f, 0.0f, 0.0f);
	}

	NiPoint3 CameraState::GetPlayerLocalOffsetCrawling(const NiPoint3& cameraPosLocal) {
		return this->GetPlayerLocalOffset(cameraPosLocal);
	}

	NiPoint3 CameraState::GetPlayerLocalOffset(const NiPoint3& cameraPosLocal, bool IsCrawling) {
		if (IsCrawling) {
			return this->GetPlayerLocalOffsetCrawling(cameraPosLocal);
		} else {
			return this->GetPlayerLocalOffset(cameraPosLocal);
		}
	}

	bool CameraState::PermitManualEdit() {
		return true;
	}

	bool CameraState::PermitTransition() {
		return true;
	}

	bool CameraState::PermitCameraTransforms() {
		return true;
	}
}
