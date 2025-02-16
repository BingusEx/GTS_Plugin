#include "Managers/Cameras/FPState.hpp"
#include "Managers/Cameras/CamUtil.hpp"

namespace GTS {

	void FirstPersonCameraState::ExitState() {
		// Reset the override
		auto player = GetCameraActor();
		auto playerData = Persistent::GetSingleton().GetData(player);
		if (playerData) {
			playerData->scaleOverride = -1.0f;
		}
	}

	float FirstPersonCameraState::GetScale() {
		// Dont need to scale as we follow the head bone in fp
		return 1.0f;
	}

	bool FirstPersonCameraState::PermitManualEdit() {
		return false;
	}

	bool FirstPersonCameraState::PermitTransition() {
		return false;
	}

	bool FirstPersonCameraState::PermitCameraTransforms() {
		return false;
	}
}
