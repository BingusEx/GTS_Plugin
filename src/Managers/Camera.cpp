#include "Managers/Camera.hpp"

#include "Managers/Cameras/CamUtil.hpp"
#include "Managers/Input/InputManager.hpp"
#include "API/SmoothCam.hpp"

#include "Config/Config.hpp"

#include "UI/ImGui/ImUtil.hpp"

using namespace GTS;

namespace {

	void HorizontalResetEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		camera.ResetLeftRight();
	}

	void VerticalResetEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		camera.ResetUpDown();
	}

	void CamUpEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustUpDown(0.6f + (size * 0.05f - 0.05f));
	}

	void CamDownEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustUpDown(-(0.6f + (size * 0.05f - 0.05f)));
	}

	void CamLeftEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustLeftRight(-(0.6f + (size * 0.05f - 0.05f)));
	}

	void CamRightEvent(const ManagedInputEvent& data) {
		auto& camera = CameraManager::GetSingleton();
		float size = get_visual_scale(PlayerCharacter::GetSingleton());
		camera.AdjustLeftRight(0.6f + (size * 0.05f - 0.05f));
	}

	void SwitchCameraMode(const ManagedInputEvent& data) {
		int& Mode = Persistent::GetSingleton().TrackedCameraState.value;

		// Increment by 1 and wrap around if we exceed the enum range
		constexpr int totalModes = static_cast<int>(magic_enum::enum_count<CameraModeTP>());
		Mode = (Mode + 1) % totalModes;

		// Convert the integer mode to the corresponding enum value
		auto currentMode = static_cast<CameraModeTP>(Mode);

		// Use magic_enum to get the enum name and humanize it
		const std::string Msg = fmt::format("Camera Mode: {}", ImUtil::HumanizeString(std::string(magic_enum::enum_name(currentMode))));

		RE::DebugNotification(Msg.c_str());
	}

	bool AutoCamEnabledCondition() {
		return Config::GetCamera().bAutomaticCamera;
	}

}

namespace GTS {

	CameraManager& CameraManager::GetSingleton() noexcept {
		static CameraManager instance;
		return instance;
	}

	std::string CameraManager::DebugName() {
		return "CameraManager";
	}

	void CameraManager::DataReady() {
		InputManager::RegisterInputEvent("HorizontalCameraReset", HorizontalResetEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("VerticalCameraReset", VerticalResetEvent, AutoCamEnabledCondition);

		InputManager::RegisterInputEvent("CameraUp", CamUpEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraDown", CamDownEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraLeft", CamLeftEvent, AutoCamEnabledCondition);
		InputManager::RegisterInputEvent("CameraRight", CamRightEvent, AutoCamEnabledCondition);

		//Ported From Papyrus
		InputManager::RegisterInputEvent("SwitchCameraMode", SwitchCameraMode, AutoCamEnabledCondition);

	}

	void CameraManager::Start() {

	}

	void CameraManager::CameraUpdate() {

		auto profiler = Profilers::Profile("Camera: Update");
		CameraState* currentState = this->GetCameraState();

		if (SmoothCam::Enabled()) {
			if (auto TPState = reinterpret_cast<ThirdPersonCameraState*>(GetCameraStateTP())) {
				if ((TPState == &this->footLState ||
					TPState == &this->footRState  ||
					TPState == &this->footState   ||
					!TPState->GetBoneTarget().boneNames.empty())) { //Checks for Valid states when using Normal or Alt Cam
					//Take control from SC so we can do our own thing if one of these conditions match
					SmoothCam::RequestConrol();
				}
				else {
					//If not in one of the above states. Return camera control to SC.
					SmoothCam::ReturnControl();
					return;
				}
			}
		}

		if (currentState != this->currentState) {
			if (this->currentState) {
				this->currentState->ExitState();
			}
			if (currentState) {
				currentState->EnterState();
			}
			auto prevState = this->currentState;
			this->currentState = currentState;
			if (prevState) {
				if (currentState) {
					if (currentState->PermitTransition() && prevState->PermitTransition()) {
						this->transitionState.reset(new TransState(prevState, currentState));
						currentState = this->transitionState.get();
					} else {
						this->transitionState.reset(nullptr);
					}
				} else {
					this->transitionState.reset(nullptr);
				}
			} else {
				this->transitionState.reset(nullptr);
			}
		} else {
			if (this->transitionState) {
				if (!this->transitionState->IsDone()) {
					currentState = this->transitionState.get();
				} else {
					this->transitionState.reset(nullptr);
				}
			}
		}

		// Handles updating the camera
		if (currentState) {

			auto player = PlayerCharacter::GetSingleton();
			bool IsCurrentlyCrawling = IsCrawling(player);
			if (IsGtsBusy(player) && IsCrawling(player) && GetCameraOverride(player)) {
				IsCurrentlyCrawling = false;
			} else if (IsProning(player)) {
				IsCurrentlyCrawling = true;
			}

			// Get scale based on camera state
			float scale = currentState->GetScale();

			// Do any scale overrides
			auto playerData = Persistent::GetSingleton().GetData(player);
			if (playerData) {
				playerData->scaleOverride = currentState->GetScaleOverride(IsCurrentlyCrawling);
			}

			// Get current camera position in player space
			auto cameraPosLocal = GetCameraPosLocal();

			// Get either normal or combat offset
			NiPoint3 offset;
			if (player != nullptr && player->AsActorState()->IsWeaponDrawn()) {
				offset = currentState->GetCombatOffset(cameraPosLocal, IsCurrentlyCrawling);
			} else {
				offset = currentState->GetOffset(cameraPosLocal, IsCurrentlyCrawling);
			}

			NiPoint3 playerLocalOffset = currentState->GetPlayerLocalOffset(cameraPosLocal, IsCurrentlyCrawling);

			//offset.z += HighHeelOffset();

			if (currentState->PermitManualEdit()) {
				this->smoothOffset.target = this->manualEdit;
			}

			offset += this->smoothOffset.value;
			this->smoothScale.target = scale;

			// Apply camera scale and offset
			if (currentState->PermitCameraTransforms()) {
				UpdateCamera(this->smoothScale.value, offset, playerLocalOffset);
			}
		}
	}

	CameraState* CameraManager::GetCameraStateTP() {

		auto Mode = static_cast<CameraModeTP>(Persistent::GetSingleton().TrackedCameraState.value);

		switch (Mode) {

			case CameraModeTP::kNormal: {
				return &this->normalState;
			}

			case CameraModeTP::kAlternative: {
				return &this->altState;
			}

			case CameraModeTP::kFeetCenter: {
				return &this->footState;
			}

			case CameraModeTP::kFootLeft: {
				return &this->footLState;
			}

			case CameraModeTP::kFootRight: {
				return &this->footRState;
			}

			default: {
				return nullptr;
			}
		}
	}

	CameraState* CameraManager::GetCameraStateFP() {
		// First Person states
		// 0 is normal
		// 1 is combat
		// 2 is loot
		int FirstPersonMode = Runtime::GetInt("FirstPersonMode");
		switch (FirstPersonMode) {
			case 0: {
				return &this->fpState;
			}
			case 1: {
				return &this->fpCombatState;
			}
			case 2: {
				return &this->fpLootState;
			}
			default: {
				return nullptr;
			}
		}
	}

	// Decide which camera state to use
	CameraState* CameraManager::GetCameraState() {

		if (!Config::GetCamera().bAutomaticCamera || IsFreeCameraEnabled()) {
			return nullptr;
		}

		bool AllowFpCamera = true;
		auto playerCamera = PlayerCamera::GetSingleton();
		if (!playerCamera) {
			return nullptr;
		}

		if (Config::GetGeneral().bConversationCamCompat) {
			auto ui = RE::UI::GetSingleton();
			if (ui) {
				if (ui->IsMenuOpen(DialogueMenu::MENU_NAME)) {
					return nullptr;
				}
			}
		}

		auto playerCameraState = playerCamera->currentState;
		if (!playerCameraState) {
			return nullptr;
		}
		RE::CameraState playerCameraMode = playerCameraState->id;

		switch (playerCameraMode) {
			// Fp state
			case RE::CameraState::kFirstPerson: {
				if (AllowFpCamera) {
					return this->GetCameraStateFP();
				} else {
					return nullptr;
				}
			}
			// All these are TP like states
			case RE::CameraState::kThirdPerson:
			case RE::CameraState::kAutoVanity:
			case RE::CameraState::kFurniture:
			case RE::CameraState::kMount:
			case RE::CameraState::kBleedout:
			case RE::CameraState::kDragon: {
				return this->GetCameraStateTP();
			}
			// These ones should be scaled but not adjusted
			// any other way like pointing at feet when using
			// kIronSights
			case RE::CameraState::kVATS:
			case RE::CameraState::kFree:
			case RE::CameraState::kPCTransition:
			case RE::CameraState::kIronSights: {
				return &this->scaledVanillaState;
			}
			// These should not be touched at all
			case RE::CameraState::kTween:
			case RE::CameraState::kAnimated: {
				return nullptr;
			}
			// Catch all in case I forgot something
			default: {
				return nullptr;
			}
		}
	}

	void CameraManager::AdjustUpDown(float amt) {
		this->manualEdit.z += amt;
	}
	void CameraManager::ResetUpDown() {
		this->manualEdit.z = 0.0f;
	}

	void CameraManager::AdjustLeftRight(float amt) {
		this->manualEdit.x += amt;
	}
	void CameraManager::ResetLeftRight() {
		this->manualEdit.x = 0.0f;
	}
}
