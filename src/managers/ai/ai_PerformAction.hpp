#pragma once









namespace GTS {
    void AI_StrongStomp(Actor* pred, Actor* prey, int rng);
    void AI_LightStomp(Actor* pred, Actor* prey, int rng);
    void AI_Tramples(Actor* pred, int rng);
    void AI_Kicks(Actor* pred, int rng);
    void AI_ButtCrush(Actor* pred, Actor* prey);
}