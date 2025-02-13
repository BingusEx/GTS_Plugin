#pragma once

// #define ENABLED_SHADOW



namespace Gts {
	BoneTarget GetBoneTargets(CameraTracking Camera_Anim, CameraTracking_MCM Camera_MCM);

	float HighHeelOffset();

	void SetINIFloat(std::string_view name, float value);

	float GetINIFloat(std::string_view name);

	void EnsureINIFloat(std::string_view name, float value);

	void UpdateThirdPerson();

	void ResetIniSettings();

	NiCamera* GetNiCamera();
	void UpdateWorld2ScreetMat(NiCamera* niCamera);

	Actor* GetCameraActor();

	/*#ifdef ENABLED_SHADOW
	   ShadowSceneNode* GetShadowMap();
	 #endif*/

	NiMatrix3 GetCameraRotation();

	void UpdateSceneManager(NiPoint3 camLoc);

	void UpdateRenderManager(NiPoint3 camLoc);

	void UpdateNiCamera(NiPoint3 camLoc);

	void UpdatePlayerCamera(NiPoint3 camLoc);

	NiMatrix3 QuatToMatrix(const NiQuaternion& q);

	NiPoint3 FirstPersonPoint();

	NiPoint3 ThirdPersonPoint();

	float ZoomFactor();
	float MaxZoom();
	NiPoint3 CompuleLookAt(float zoomScale = 0.95f);

	// Get's camera position relative to the player
	NiPoint3 GetCameraPosLocal();

	void UpdateCamera(float scale, NiPoint3 cameraLocalOffset, NiPoint3 playerLocalOffset);
}
