#include "Utils/Camera.hpp"

namespace GTS {

	void shake_camera_script(TESObjectREFR* actor, float intensity, float duration) { // TESObjectREFR*
		CallFunction("Game", "ShakeCamera", actor, intensity, duration);
	}

	void shake_camera(Actor* actor, float intensity, float duration) { // TESObjectREFR*
		//CallFunction("Game", "ShakeCamera", actor, intensity, duration);
		auto node = find_node(actor, "NPC COM [COM ]");
		if (node) {
			NiPoint3 position = node->world.translate;
			ShakeCamera(intensity, position, duration);
		}
	}

	void shake_camera_at_node(NiPoint3 position, float intensity, float duration) { // TESObjectREFR*
		ShakeCamera(intensity, position, duration);
	}

	void shake_camera_at_node(Actor* giant, std::string_view node, float intensity, float duration) { // TESObjectREFR*
		auto bone = find_node(giant, node);
		if (bone) {
			NiPoint3 position = bone->world.translate;
			ShakeCamera(intensity, position, duration);
		}
	}

	void TriggerScreenBlood(int aiValue) {
		CallFunction("Game", "TriggerScreenBlood", aiValue);
	}

	void shake_controller(float left_intensity, float right_intensity, float duration) {
		CallFunction("Game", "ShakeController", left_intensity, right_intensity, duration);
	}

	float get_distance_to_camera(const NiPoint3& point) {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto &point_a = point;
			auto point_b = camera->pos;
			auto delta = point_a - point_b;
			return delta.Length();
		}
		//return 3.4028237E38; // Max float //Throws overflow warning
		return std::numeric_limits<float>::max(); // Max float
	}

	float get_distance_to_camera_no_Z(const NiPoint3& point) {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto &point_a = point;
			auto point_b = camera->pos;
			
			//point_a.z = 0;
			//point_b.z = 0;

			auto delta = point_a - point_b;
			return delta.Length();
		}
		//return 3.4028237E38; // Max float //Throws overflow warning
		return std::numeric_limits<float>::max(); // Max float
	}

	float get_distance_to_camera(NiAVObject* node) {
		if (node) {
			return get_distance_to_camera(node->world.translate);
		}
		//return 3.4028237E38; // Max float //Throws overflow warning
		return std::numeric_limits<float>::max(); // Max float
	}

	float get_distance_to_camera(Actor* actor) {
		if (actor) {
			return get_distance_to_camera(actor->GetPosition());
		}
		//return 3.4028237E38; // Max float //Throws overflow warning
		return std::numeric_limits<float>::max(); // Max float
	}

	bool IsFirstPerson() {
		auto playercamera = PlayerCamera::GetSingleton();
		if (!playercamera) {
			return false;
		}
		if (playercamera->currentState == playercamera->cameraStates[CameraState::kFirstPerson]) {
			return true;
		}
		return false;
	}

	bool HasFirstPersonBody() {
		auto camera = RE::PlayerCamera::GetSingleton();
		if (camera->currentState) {
			std::uint32_t cameraID = camera->currentState->id;
			if (cameraID == RE::CameraState::kThirdPerson) {
				auto thirdPersonState = static_cast<RE::ThirdPersonState*>(camera->cameraStates[cameraID].get());
				if (thirdPersonState && thirdPersonState->currentZoomOffset <= -0.275f) {
					return true;
				}
			} else if (cameraID == RE::CameraState::kFirstPerson) {
				auto actor3D = RE::PlayerCharacter::GetSingleton()->Get3D(false);
				if (actor3D && !actor3D->GetFlags().any(RE::NiAVObject::Flag::kHidden) & 1) {
					return true;
				}
			}
		}

		// Reports TRUE if we're using IFPV first person mode
				
		return false;
	}

	bool IsFakeFirstPerson() {
		auto camera = PlayerCamera::GetSingleton();
		if (camera) {
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(camera->cameraStates[RE::CameraStates::kThirdPerson].get());
			if (thirdPersonState) {
				auto currentZoom = thirdPersonState->currentZoomOffset;
				if (currentZoom == -0.275f) {
					return true;
				}
			}
		}
		return false;
	}

	void ForceThirdPerson(Actor* giant) {
		if (giant->formID == 0x14) {
			auto camera = RE::PlayerCamera::GetSingleton();
			if (camera) {
				camera->ForceThirdPerson();
				auto playerCamera = RE::PlayerCamera::GetSingleton();
				auto thirdPersonState = reinterpret_cast<RE::ThirdPersonState*>(playerCamera->cameraStates[RE::CameraState::kThirdPerson].get());
				auto isInThirdPerson = playerCamera->currentState->id == RE::CameraState::kThirdPerson;

				TaskManager::RunOnce([=](auto& update){
					log::info("Running Camera Task Once");
					if (thirdPersonState && isInThirdPerson) {
						log::info("Applying zoom offset");
						thirdPersonState->currentZoomOffset = 0.50f;
						thirdPersonState->targetZoomOffset = 0.50f;
					}
				});
				
				ActorHandle giantHandle = giant->CreateRefHandle();

				double start = Time::WorldTimeElapsed();

				TaskManager::Run([=](auto& update) {
					if (!giantHandle) {
						return false;
					}
					Actor* giantref = giantHandle.get().get();
					bool Busy = IsGtsBusy(giantref);
					if (!Busy) {
						if (Time::WorldTimeElapsed() - start < 0.15) {
							return true;
						}
						if (thirdPersonState && !IsFirstPerson()) {
							camera->ForceFirstPerson();
							thirdPersonState->currentZoomOffset = 0.0f;
							thirdPersonState->targetZoomOffset = 0.0f;
							return false;
						} else {
							return true;
						}
						return false;
					}
					return true;
				});
			}
		}
	}
}
