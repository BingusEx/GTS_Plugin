#include "managers/animation/AnimationManager.hpp"
#include "managers/animation/ThighSandwich.hpp"
#include "managers/emotions/EmotionManager.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/GtsSizeManager.hpp"
#include "Managers/Input/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/explosion.hpp"
#include "managers/tremor.hpp"
#include "managers/Rumble.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"




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

	void Task_UpdatePhenome(Actor* giant, int phenome, float mfg_speed, float target) {
		
		if (!EmotionManager::IsEmotionBusy(giant, CharEmotionType::Phenome)) {
			std::string name = std::format("Phenome_{}_{}_{}_{}", giant->formID, phenome, target, Time::WorldTimeElapsed());
			ActorHandle giantHandle = giant->CreateRefHandle();
			double start = Time::WorldTimeElapsed();

			float InitialValue = EmotionManager::GetEmotionValue(giant, CharEmotionType::Phenome, phenome);
			bool revert = target <= 0.0f;

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
				float speed = 1.25f * AnimSpeed * mfg_speed * Speed_up;
				//log::info("Running Facial Task: {}", name);
				float value = static_cast<float>(pass * speed);
				auto FaceData = GetFacialData(giantref);

				bool ShouldFinish = target > 0.0f && value >= target;

				if (FaceData) {
					if (ShouldFinish) { // fully applied
						Phenome_ManagePhenomes(FaceData, phenome, target);
						EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
						return false;
					} 

					if (revert) {
						Phenome_ManagePhenomes(FaceData, phenome, InitialValue - value);
						if (InitialValue - value <= 0.0f) {
							Phenome_ManagePhenomes(FaceData, phenome, 0.0f); // Finish Task
							EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
							return false;
						}
					} else {
						Phenome_ManagePhenomes(FaceData, phenome, value);
					}
					
					return true;
				}
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Phenome, false);
				return false;
			});
		}
	}

	void Task_UpdateModifier(Actor* giant, int modifier, float mfg_speed, float target) {
		if (!EmotionManager::IsEmotionBusy(giant, CharEmotionType::Modifier)) {
			std::string name = std::format("Modifier_{}_{}_{}", giant->formID, modifier, target);
			ActorHandle giantHandle = giant->CreateRefHandle();
			double start = Time::WorldTimeElapsed();

			float InitialValue = EmotionManager::GetEmotionValue(giant, CharEmotionType::Modifier, modifier);
			bool revert = target <= 0.0f;

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
				float speed = 1.0f * AnimSpeed * mfg_speed * Speed_up;

				float value = static_cast<float>(pass * speed);
				auto FaceData = GetFacialData(giantref);

				bool ShouldFinish = target > 0.0f && value >= target;
				//log::info("Running Facial Task: {}", name);
				if (FaceData) {
					if (ShouldFinish) { // fully applied
						Phenome_ManageModifiers(FaceData, modifier, target);
						EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);
						return false;
					} 

					if (revert) {
						Phenome_ManageModifiers(FaceData, modifier, InitialValue - value);
						if (InitialValue - value <= 0.0f) {
							Phenome_ManageModifiers(FaceData, modifier, 0.0f); // Finish Task
							EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);
							return false;
						}
					} else {
						Phenome_ManageModifiers(FaceData, modifier, value);
					}

					return true;
				}
				EmotionManager::SetEmotionBusy(giantref, CharEmotionType::Modifier, false);
				return false;
			});
		}
	}
}

namespace GTS {

	EmotionManager& EmotionManager::GetSingleton() noexcept {
		static EmotionManager instance;
		return instance;
	}

	std::string EmotionManager::DebugName() {
		return "EmotionManager";
	}

	void EmotionManager::SetEmotionBusy(Actor* giant, CharEmotionType Type, bool lock) { // We don't want emotion tasks to stack, it breaks them
		auto data = Transient::GetSingleton().GetActorData(giant);
		if (data) {
			switch (Type) {
				case CharEmotionType::Modifier:
					data->emotion_modifier_busy = lock;
				break;
				case CharEmotionType::Phenome:
					data->emotion_phenome_busy = lock;
				break;
			}
		}
	}

	bool EmotionManager::IsEmotionBusy(Actor* giant, CharEmotionType Type) {
		bool busy = false;
		auto data = Transient::GetSingleton().GetActorData(giant);
		if (data) {
			switch (Type) {
				case CharEmotionType::Modifier:
					busy = data->emotion_modifier_busy;
				break;
				case CharEmotionType::Phenome:
					busy = data->emotion_phenome_busy;
				break;
			}
		}
		return busy;
	}

	float EmotionManager::GetEmotionValue(Actor* giant, CharEmotionType Type, std::uint32_t emotion_value) {
		float value = 0.0;
		auto data = GetFacialData(giant);
		if (data) {
			switch (Type) {
				case CharEmotionType::Modifier: 
					value = Phenome_GetModifierValue(data, emotion_value);
				break;
				case CharEmotionType::Phenome:
					value = Phenome_GetPhenomeValue(data, emotion_value);
				break;
			}
		}
		return value;
	}

	void EmotionManager::OverridePhenome(Actor* giant, int number, float mfg_speed, float target) {
		Task_UpdatePhenome(giant, number, mfg_speed, target);
	}

	void EmotionManager::OverrideModifier(Actor* giant, int number, float mfg_speed, float target) {
		Task_UpdateModifier(giant, number, mfg_speed, target);
	}
}