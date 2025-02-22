#include "Scale/Scale.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {
	constexpr std::string_view PapyrusClass = "GtsScale";
	void ResetScales() {
		for (auto actor: find_actors()) {
			if (actor) {
				ResetToInitScale(actor);
			}
		}
	}

	// Model Scale
	bool SetModelScale(StaticFunctionTag*, Actor* actor, float scale) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			result = set_model_scale(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}
	float GetModelScale(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto result = get_model_scale(actor);
		return result;
	}
	bool ModModelScale(StaticFunctionTag*, Actor* actor, float amt) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			auto scale = get_model_scale(actor) + amt;
			result = set_model_scale(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	// Node Scale
	bool SetNodeScale(StaticFunctionTag*, Actor* actor, float scale) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			result = set_npcnode_scale(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}
	float GetNodeScale(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto result = get_npcnode_scale(actor);
		return result;
	}
	bool ModNodeScale(StaticFunctionTag*, Actor* actor, float amt) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			auto scale = get_npcnode_scale(actor) + amt;
			result = set_npcnode_scale(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	// Ref scale
	bool SetHybridScale(StaticFunctionTag*, Actor* actor, float scale) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			//set_ref_scale(actor, scale);
			if (actor->formID == 0x14) {
				result = set_npcnode_scale(actor, scale);
			} else if (actor->formID != 0x14) {
				result = set_model_scale(actor, scale);
			}
			//result = true; // Ref scale cannot fail
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	float GetHybridScale(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto result = get_model_scale(actor); // If not player, get Model
		if (actor->formID == 0x14) {
			result = get_npcnode_scale(actor); // else get node
		}
		return result;
	}

	bool ModHybridScale(StaticFunctionTag*, Actor* actor, float amt) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			auto scale = get_model_scale(actor) + amt;
			if (actor->formID == 0x14) { // If player, mod Node
				scale = get_npcnode_scale(actor);
				result = set_npcnode_scale(actor, scale);
			} else if (actor->formID != 0x14) { // Else mod Model
				scale = get_model_scale(actor);
				result = set_model_scale(actor, scale);
			}

			//set_ref_scale(actor, scale);
			//result = true;
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	//STUBBED OUT
	void SetScaleMethod(StaticFunctionTag*, int size_method) {
		return;
	}

	//STUBBED OUT
	int GetScaleMethod(StaticFunctionTag*) {
		return 0;
	}

	bool SetScale(StaticFunctionTag*, Actor* actor, float scale) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			result = update_model_visuals(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	float GetScale(StaticFunctionTag*, Actor* actor) {
		if (!actor) {
			return 0.0f;
		}
		auto result = get_scale(actor);
		return result;
	}

	bool ModScale(StaticFunctionTag*, Actor* actor, float amt) {
		bool result = false;
		auto actor_data = Persistent::GetSingleton().GetData(actor);
		if (actor_data) {
			auto scale = get_scale(actor) + amt;
			result = update_model_visuals(actor, scale);
			actor_data->visual_scale = scale;
			actor_data->visual_scale_v = 0.0f;
			actor_data->target_scale = scale;
		}
		return result;
	}

	// Target Scales
	void SetTargetScale(StaticFunctionTag*, Actor* actor, float scale) {
		set_target_scale(actor, scale);
	}

	float GetTargetScale(StaticFunctionTag*, Actor* actor) {
		return get_target_scale(actor);
	}

	void ModTargetScale(StaticFunctionTag*, Actor* actor, float amt) {
		mod_target_scale(actor, amt);
	}

	void SetMaxScale(StaticFunctionTag*, Actor* actor, float scale) {
		set_max_scale(actor, scale);
	}

	float GetMaxScale(StaticFunctionTag*, Actor* actor) {
		return get_max_scale(actor); 
		// Naturally we / the scale by natural scale inside GtsSizeManager.cpp, so we compensate it here
		// Else it will report x3.0 instead of x8.0 for example (when the SetScale is x2.5)
	}

	void ModMaxScale(StaticFunctionTag*, Actor* actor, float amt) {
		mod_max_scale(actor, amt);
	}

	float GetVisualScale(StaticFunctionTag*, Actor* actor) {
		return get_visual_scale(actor);
	}

	void ModTeammateScale(StaticFunctionTag*, float amt) {
		for (auto actor: find_actors()) {
			if (!actor) {
				continue;
			}
			if (!actor->Is3DLoaded()) {
				continue;
			}
			if (IsTeammate(actor)) {
				mod_target_scale(actor, amt);
			}
		}
	}

	float GetOtherScale(StaticFunctionTag*, Actor* actor) {
		return get_natural_scale(actor);
	}

	float GetGiantessScale(StaticFunctionTag*, Actor* actor) {
		return get_giantess_scale(actor);
	}

	float GetNaturalScale(StaticFunctionTag*, Actor* actor) {
		return get_natural_scale(actor, true);
	}
}

namespace GTS {
	bool register_papyrus_scale(IVirtualMachine* vm) {
		vm->RegisterFunction("SetModelScale", PapyrusClass, SetModelScale);
		vm->RegisterFunction("GetModelScale", PapyrusClass, GetModelScale);
		vm->RegisterFunction("ModModelScale", PapyrusClass, ModModelScale);

		vm->RegisterFunction("SetNodeScale", PapyrusClass, SetNodeScale);
		vm->RegisterFunction("GetNodeScale", PapyrusClass, GetNodeScale);
		vm->RegisterFunction("ModNodeScale", PapyrusClass, ModNodeScale);

		vm->RegisterFunction("SetHybridScale", PapyrusClass, SetHybridScale);
		vm->RegisterFunction("GetHybridScale", PapyrusClass, GetHybridScale);
		vm->RegisterFunction("ModHybridScale", PapyrusClass, ModHybridScale);

		vm->RegisterFunction("SetScaleMethod", PapyrusClass, SetScaleMethod);
		vm->RegisterFunction("GetScaleMethod", PapyrusClass, GetScaleMethod);
		vm->RegisterFunction("SetScale", PapyrusClass, SetScale);
		vm->RegisterFunction("GetScale", PapyrusClass, GetScale);
		vm->RegisterFunction("ModScale", PapyrusClass, ModScale);

		vm->RegisterFunction("SetTargetScale", PapyrusClass, SetTargetScale);
		vm->RegisterFunction("GetTargetScale", PapyrusClass, GetTargetScale);
		vm->RegisterFunction("ModTargetScale", PapyrusClass, ModTargetScale);

		vm->RegisterFunction("SetMaxScale", PapyrusClass, SetMaxScale);
		vm->RegisterFunction("GetMaxScale", PapyrusClass, GetMaxScale);
		vm->RegisterFunction("ModMaxScale", PapyrusClass, ModMaxScale);

		vm->RegisterFunction("GetVisualScale", PapyrusClass, GetVisualScale);

		vm->RegisterFunction("ModTeammateScale", PapyrusClass, ModTeammateScale);

		vm->RegisterFunction("GetOtherScale", PapyrusClass, GetOtherScale);

		vm->RegisterFunction("GetGiantessScale", PapyrusClass, GetGiantessScale);

		vm->RegisterFunction("GetNaturalScale", PapyrusClass, GetNaturalScale);

		return true;
	}
}
