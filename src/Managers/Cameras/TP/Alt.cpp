#include "Managers/Cameras/TP/Alt.hpp"
#include "Managers/Cameras/Camutil.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Config/Config.hpp"

namespace GTS {

	auto& CamSettings = Config::GetCamera().OffsetsAlt;

	NiPoint3 Alt::GetOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalStand[0],
			CamSettings.f3NormalStand[1],
			CamSettings.f3NormalStand[2],
		};

	}

	NiPoint3 Alt::GetCombatOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatStand[0],
			CamSettings.f3CombatStand[1],
			CamSettings.f3CombatStand[2],
		};

	}

	NiPoint3 Alt::GetOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalSneak[0],
			CamSettings.f3NormalSneak[1],
			CamSettings.f3NormalSneak[2],
		};

	}

	NiPoint3 Alt::GetCombatOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatSneak[0],
			CamSettings.f3CombatSneak[1],
			CamSettings.f3CombatSneak[2],
		};

	}

	BoneTarget Alt::GetBoneTarget() {
		auto player = PlayerCharacter::GetSingleton();
		auto& sizemanager = SizeManager::GetSingleton();

		CameraTracking Camera_Anim = sizemanager.GetTrackedBone(player);
		return GetBoneTargets(Camera_Anim, StringToEnum<CameraTrackingSettings>(CamSettings.sCenterOnBone));
	}
}
