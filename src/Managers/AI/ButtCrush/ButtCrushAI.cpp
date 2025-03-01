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

#include "Managers/AI/ButtCrush/ButtCrushAI.hpp"
