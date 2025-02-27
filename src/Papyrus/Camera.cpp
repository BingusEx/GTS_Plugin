#include "Papyrus/Camera.hpp"
#include "Managers/Cameras/Camutil.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GtsCamera";

	void ToggleFreeCamera(StaticFunctionTag*) {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			camera->ToggleFreeCameraMode(false);
		}
	}
}

namespace GTS {
	bool register_papyrus_camera(IVirtualMachine* vm) {
		vm->RegisterFunction("ToggleFreeCamera", PapyrusClass, ToggleFreeCamera);
		return true;
	}
}
