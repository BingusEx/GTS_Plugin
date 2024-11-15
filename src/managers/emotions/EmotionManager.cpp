#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/ThighSandwich.hpp"
#include "managers/emotions/EmotionManager.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "managers/explosion.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/tremor.hpp"
#include "managers/Rumble.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "profiler.hpp"
#include "spring.hpp"
#include "node.hpp"

namespace {

	const float Speed_up = 12.0f;

	BSFaceGenAnimationData* GetFacialData(Actor* giant) {
		auto fgen = giant->GetFaceGenAnimationData();
		if (fgen) {
			return fgen;
		}
		return nullptr;
	}

	float Phenome_GetPhenomeValue(BSFaceGenAnimationData* data, std::uint32_t Phenome) {
		float value = data->phenomeKeyFrame.values[Phenome];
		return value;
	}

	float Phenome_GetModifierValue(BSFaceGenAnimationData* data, std::uint32_t Modifier) {
		float value = data->modifierKeyFrame.values[Modifier];
		return value;
	}

	void Phenome_ManagePhenomes(BSFaceGenAnimationData* data, std::uint32_t Phenome, float Value) {
		data->phenomeKeyFrame.SetValue(Phenome, Value);
	}

	void Phenome_ManageModifiers(BSFaceGenAnimationData* data, std::uint32_t Modifier, float Value) {
		data->modifierKeyFrame.SetValue(Modifier, Value);
	}

	void Task_UpdatePhenome(Actor* giant, int phenome, float halflife, float target) {
		std::string name = std::format("Phenome_{}_{}_{}_{}", giant->formID, phenome, target, Time::WorldTimeElapsed());
		ActorHandle giantHandle = giant->CreateRefHandle();

		float modified = 0.0f;
		auto data = GetFacialData(giant);
		if (data) {
			modified = Phenome_GetPhenomeValue(data, phenome);
		}
		
		double start = Time::WorldTimeElapsed();

		bool Reset = (target < 0.01f);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}

			auto giantref = giantHandle.get().get();
			double pass = Time::WorldTimeElapsed() - start;

			if (!giantref->Is3DLoaded()) {
				return false;
			}

			float AnimSpeed = AnimationManager::GetSingleton().GetAnimSpeed(giant);
			float speed = 1.25f * AnimSpeed * halflife * Speed_up;
			//log::info("Running Facial Task: {}", name);
			float value = static_cast<float>(pass * speed);
			auto FaceData = GetFacialData(giantref);
			if (FaceData) {
				if (Reset && modified != 0.0f) {
					value = modified - static_cast<float>(pass * speed);
					Phenome_ManagePhenomes(FaceData, phenome, value);
					if (value <= 0.0f) {
						Phenome_ManagePhenomes(FaceData, phenome, 0.0f);
						return false;
					}
					return true;
				} if (value >= target) { // fully applied
					Phenome_ManagePhenomes(FaceData, phenome, target);
					return false;
				} 

				Phenome_ManagePhenomes(FaceData, phenome, value);
				return true;
			}

			return false;
		});
	}

	void Task_UpdateModifier(Actor* giant, int modifier, float halflife, float target) {

		std::string name = std::format("Modifier_{}_{}_{}", giant->formID, modifier, target);

		float modified = 0.0f;

		auto data = GetFacialData(giant);
		if (data) {
			modified = Phenome_GetModifierValue(data, modifier);
		}

		ActorHandle giantHandle = giant->CreateRefHandle();
		

		double start = Time::WorldTimeElapsed();

		bool Reset = (target < 0.01f);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!giantHandle) {
				return false;
			}

			auto giantref = giantHandle.get().get();
			double pass = Time::WorldTimeElapsed() - start;

			if (!giantref->Is3DLoaded()) {
				return false;
			}

			float AnimSpeed = AnimationManager::GetSingleton().GetAnimSpeed(giant);
			float speed = 1.0f * AnimSpeed * halflife * Speed_up;

			float value = static_cast<float>(pass * speed);
			auto FaceData = GetFacialData(giantref);
			//log::info("Running Facial Task: {}", name);
			if (FaceData) {
				if (Reset) {
					value = modified - static_cast<float>(pass * speed);
					Phenome_ManageModifiers(FaceData, modifier, value);
					if (value <= 0 || value >= target) {
						Phenome_ManageModifiers(FaceData, modifier, 0.0f);
						return false;
					} if (value == target) {
						return false;
					}
					return true;
				} if (value >= target) { // fully applied
					Phenome_ManageModifiers(FaceData, modifier, target);
					return false;
				} 

				Phenome_ManageModifiers(FaceData, modifier, value);
				return true;
			}

			return false;
		});
	}
}

namespace Gts {

	EmotionManager& EmotionManager::GetSingleton() noexcept {
		static EmotionManager instance;
		return instance;
	}

	std::string EmotionManager::DebugName() {
		return "EmotionManager";
	}

	void EmotionManager::OverridePhenome(Actor* giant, int number, float power, float halflife, float target) {
		Task_UpdatePhenome(giant, number, halflife, target);
	}

	void EmotionManager::OverrideModifier(Actor* giant, int number, float power, float halflife, float target) {
		Task_UpdateModifier(giant, number, halflife, target);
	}
}