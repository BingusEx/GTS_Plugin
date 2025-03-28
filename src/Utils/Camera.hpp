#pragma once

namespace GTS {

	void shake_camera_script(TESObjectREFR* actor, float intensity, float duration);
	void shake_camera(Actor* actor, float intensity, float duration);
	void shake_camera_at_node(NiPoint3 position, float intensity, float duration);
	void shake_camera_at_node(Actor* giant, std::string_view node, float intensity, float duration);

	void TriggerScreenBlood(int aiValue);

	void shake_controller(float left_intensity, float right_intensity, float duration);

	float get_distance_to_camera(const NiPoint3& point);

	float get_distance_to_camera_no_Z(const NiPoint3& point);

	float get_distance_to_camera(NiAVObject* node);

	float get_distance_to_camera(Actor* actor);

	bool IsFirstPerson();

	bool HasFirstPersonBody();

	bool IsFakeFirstPerson();

	void ForceThirdPerson(Actor* giant);
}
