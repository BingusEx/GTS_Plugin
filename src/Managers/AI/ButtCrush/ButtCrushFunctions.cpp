/*
void Task_ButtCrushLogicTask(Actor* giant) {

    std::string name = std::format("ButtCrush_AI_{}", giant->formID);

    auto gianthandle = giant->CreateRefHandle();
    auto FrameA = Time::FramesElapsed();
    TaskManager::Run(name, [=](auto& progressData) {
        if (!gianthandle) {
            return false;
        }
        auto FrameB = Time::FramesElapsed() - FrameA;
        if (FrameB <= 10.0f) {
            return true;
        }

        auto giantref = gianthandle.get().get();

        bool CanGrow = ButtCrush_IsAbleToGrow(giantref, GetGrowthLimit(giantref));

        bool BlockGrowth = IsActionOnCooldown(giantref, CooldownSource::Misc_AiGrowth);

        if (IsChangingSize(giantref)) { // Growing/shrinking
            ApplyActionCooldown(giantref, CooldownSource::Misc_AiGrowth);
        }

        if (BlockGrowth) {
            return true;
        }

        if (CanGrow && IsButtCrushing(giantref) && !IsChangingSize(giantref) && Runtime::HasPerkTeam(giantref, "ButtCrush_GrowingDisaster")) {
            ApplyActionCooldown(giantref, CooldownSource::Misc_AiGrowth);
            int rng = RandomInt(0, 10);
            if (rng <= 6) {
                AnimationManager::StartAnim("ButtCrush_Growth", giantref);
            }
        }
        else if (!CanGrow && !IsChangingSize(giantref)) { // Can't grow any further
            AnimationManager::StartAnim("ButtCrush_Attack", giantref);
        }

        if (!IsButtCrushing(giantref)) {
            return false; // End the task
        }
        return true;
        });
}

void AI_ButtCrush(Actor* pred, Actor* prey) {
    if (!Persistent::GetSingleton().Butt_Ai) {
        return;
    }
    if (IsActionOnCooldown(pred, CooldownSource::Action_ButtCrush)) {
        return;
    }
    if (IsGtsBusy(pred) || IsChangingSize(pred)) {
        return;
    }
    auto grabbedActor = Grab::GetHeldActor(pred);
    if (grabbedActor && !IsCrawling(pred)) { // If gts has someone in hands, allow only when we crawl
        return;
    }

    int rng = RandomInt(0, 10);
    if (Runtime::HasPerkTeam(pred, "ButtCrush_NoEscape") && rng > 2) {
        auto& ButtCrush = ButtCrushController::GetSingleton();

        ButtCrush.StartButtCrush(pred, prey); // attaches actors to AnimObjectB, CanButtCrush check is done inside the function
        Task_ButtCrushLogicTask(pred);
    }
    else {
        AnimationManager::StartAnim("ButtCrush_StartFast", pred);
    }
}


// butt crush related things




*/

#include "Managers/AI/ButtCrush/ButtCrushFunctions.hpp"

namespace GTS {

	float GetButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float butt_crush_size = std::clamp(saved_data->buttcrush_max_size, 0.0f, 1000000.0f);
			return butt_crush_size;
		}
		return 0.0f;
	}

	float GetGrowthCount(Actor* giant) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			return transient->ButtCrushGrowthAmount;
		}
		return 1.0f;
	}

	float GetGrowthLimit(Actor* actor) {
		float limit = 0;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_GrowingDisaster")) {
			limit += 3.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			limit += 4.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_LoomingDoom")) {
			limit += 5.0f;
		}
		return limit;
	}

	float GetButtCrushDamage(Actor* actor) {
		float damage = 1.0f;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_KillerBooty")) {
			damage += 0.30f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			damage += 0.70f;
		}
		return damage;
	}

	void ModGrowthCount(Actor* giant, float value, bool reset) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			transient->ButtCrushGrowthAmount += value;
			if (reset) {
				transient->ButtCrushGrowthAmount = 0.0f;
			}
		}
	}

	void RecordStartButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			saved_data->buttcrush_start_scale = get_target_scale(giant);
		}
	}

	void SetButtCrushSize(Actor* giant, float value, bool reset) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float scale = game_getactorscale(giant);
			saved_data->buttcrush_max_size += value * scale;

			if (!reset && GetGrowthCount(giant) <= 1) {
				RecordStartButtCrushSize(giant);
			}

			if (reset) {
				float remove_size = saved_data->buttcrush_max_size;
				float start_scale = saved_data->buttcrush_start_scale;

				start_scale = std::max(start_scale, get_natural_scale(giant));

				set_target_scale(giant, start_scale);

				saved_data->buttcrush_max_size = 0;
				saved_data->buttcrush_start_scale = 0.0f;
			}
		}
	}
}