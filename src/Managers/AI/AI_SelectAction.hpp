#pragma once

namespace GTS {

    void AI_TryAction(Actor* actor);
    void AI_DoStomp_Kick_ButtCrush(Actor* pred);
    void AI_DoSandwich(Actor* pred);
    void AI_DoHugs(Actor* pred);
    void AI_StartHugs(Actor* pred, Actor* prey);
    void AI_StartHugsTask(Actor* giant, Actor* tiny);
    void AI_DoThighCrush(Actor* giant);
    void AI_StartThighCrushTask(Actor* giant);

}