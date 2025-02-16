#include "Managers/AI/AI_PerformAction.hpp"
#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Animation/AnimationManager.hpp"
#include "Managers/Animation/Grab.hpp"
#include "Managers/Animation/Stomp_Under.hpp"
#include "Managers/Animation/Controllers/ButtCrushController.hpp"
#include "Managers/Animation/Utils/AnimationUtils.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"


using namespace GTS;

namespace {

    const std::vector<std::string_view> light_kicks = {
        "SwipeLight_Left",                  // 0
        "SwipeLight_Right",                 // 1
        "SwipeLight_Right",                 // 2, a fail-safe one in case random does funny stuff 
    };

    const std::vector<std::string_view> heavy_kicks = {
        "SwipeHeavy_Right",                 // 0
        "SwipeHeavy_Left",                  // 1
        "StrongKick_Low_Right",             // 2
        "StrongKick_Low_Left",              // 3
        "StrongKick_Low_Left",              // 4, a fail-safe one in case random does funny stuff 
    };

    float GetDistanceBetween(Actor* giant, Actor* tiny) {
        float prey_distance = 0.0f;
        if (giant && tiny) {
            prey_distance = (giant->GetPosition() - tiny->GetPosition()).Length();
            prey_distance /= get_visual_scale(giant);
        }

        return prey_distance;
    }

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
            } else if (!CanGrow && !IsChangingSize(giantref)) { // Can't grow any further
                AnimationManager::StartAnim("ButtCrush_Attack", giantref);
            }

            if (!IsButtCrushing(giantref)) {
                return false; // End the task
            }
            return true;
        });
    }
    

    void AI_Heavy_Kicks(Actor* pred) {
        int rng = RandomInt(0, 3);
        if (IsCrawling(pred)) {
            rng = RandomInt(0, 1); // Always swipe
        }  
        log::info("Heavy Kicks rng for {} is {}", pred->GetDisplayFullName(), rng);
        AnimationManager::StartAnim(heavy_kicks[rng], pred);
    }
    void AI_Light_Kicks(Actor* pred) {
        int rng = RandomInt(0, 1);
        int limit = 1;
        //log::info("Light Kicks rng for {} is {}", pred->GetDisplayFullName(), rng);
        AnimationManager::StartAnim(light_kicks[rng], pred);
    }
}

namespace GTS {
    
    void AI_StrongStomp(Actor* pred, Actor* prey, int rng) {
        if (!Persistent::GetSingleton().Stomp_Ai) {
            return; // don't check any further if it is disabled
        }

        bool UnderStomp = AnimationUnderStomp::ShouldStompUnder_NPC(pred, GetDistanceBetween(pred, prey));
		const std::string_view StompType_R = UnderStomp ? "UnderStompStrongRight" : "StrongStompRight";
        const std::string_view StompType_L = UnderStomp ? "UnderStompStrongLeft" : "StrongStompLeft";

        if (rng <= 5) {
            AnimationManager::StartAnim(StompType_R, pred);
        } else {
            AnimationManager::StartAnim(StompType_L, pred);
        }
    }
    void AI_LightStomp(Actor* pred, Actor* prey, int rng) {
        if (!Persistent::GetSingleton().Stomp_Ai) {
            return; // don't check any further if it is disabled
        }
        Utils_UpdateHighHeelBlend(pred, false);
        bool UnderStomp = AnimationUnderStomp::ShouldStompUnder_NPC(pred, GetDistanceBetween(pred, prey));
		const std::string_view StompType_R = UnderStomp ? "UnderStompRight" : "StompRight";
        const std::string_view StompType_L = UnderStomp ? "UnderStompLeft" : "StompLeft";

        if (rng <= 5) {
            AnimationManager::StartAnim(StompType_R, pred);
        } else {
            AnimationManager::StartAnim(StompType_L, pred);
        }
    }

    void AI_Tramples(Actor* pred, int rng) {
        if (!Persistent::GetSingleton().Stomp_Ai) {
            return;
        }
        Utils_UpdateHighHeelBlend(pred, false);
        if (rng <= 5) {
            AnimationManager::StartAnim("TrampleL", pred);
        } else {
            AnimationManager::StartAnim("TrampleR", pred);
        }
    }

    void AI_Kicks(Actor* pred, int rng) {
        if (!Persistent::GetSingleton().Kick_Ai) {
            return;
        }
        Utils_UpdateHighHeelBlend(pred, false);
        if (rng <= 5) {
            AI_Heavy_Kicks(pred);
        } else {
            AI_Light_Kicks(pred);
        }
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
        } else {
            AnimationManager::StartAnim("ButtCrush_StartFast", pred);
        }
    }
}