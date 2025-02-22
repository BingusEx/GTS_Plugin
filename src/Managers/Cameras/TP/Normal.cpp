#include "Managers/Cameras/TP/Normal.hpp"
#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Config/Config.hpp"

namespace GTS {

	static auto& CamSettings = Config::GetCamera().OffsetsNormal;

	NiPoint3 Normal::GetOffset(const NiPoint3& cameraPos) {


		return {
			CamSettings.f3NormalStand[0],
			CamSettings.f3NormalStand[1],
			CamSettings.f3NormalStand[2],
		};

	}

	NiPoint3 Normal::GetCombatOffset(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3CombatStand[0],
			CamSettings.f3CombatStand[1],
			CamSettings.f3CombatStand[2],
		};

	}

	NiPoint3 Normal::GetOffsetProne(const NiPoint3& cameraPos) {

		return {
			CamSettings.f3NormalSneak[0],
			CamSettings.f3NormalSneak[1],
			CamSettings.f3NormalSneak[2],
		};

	}

	NiPoint3 Normal::GetCombatOffsetProne(const NiPoint3& cameraPos)  {

		return {
			CamSettings.f3CombatSneak[0],
			CamSettings.f3CombatSneak[1],
			CamSettings.f3CombatSneak[2],
		};

	}

	BoneTarget Normal::GetBoneTarget() {
		auto player = PlayerCharacter::GetSingleton();
		auto& sizemanager = SizeManager::GetSingleton();
		CameraTracking Camera_Anim = sizemanager.GetTrackedBone(player);

		return GetBoneTargets(Camera_Anim, StringToEnum<CameraTrackingSettings>(CamSettings.sCenterOnBone));
	}
}
