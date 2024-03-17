

#pragma once

#include "events.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace Gts {
    void TinyCalamity_SeekActorForShrink_Foot(Actor* actor, Actor* otherActor, float damage, float radius, int random, float bbmult, float crush_threshold, DamageSource Cause, bool Right, bool ApplyCooldown);
    void TinyCalamity_ShrinkActor(Actor* giant, Actor* tiny, float shrink);
    void TinyCalamity_ExplodeActor(Actor* giant, Actor* tiny);
    void TinyCalamity_StaggerActor(Actor* giant, Actor* tiny, float giantHp);
     
    void TinyCalamity_SeekActors(Actor* giant);
    void TinyCalamity_CrushCheck(Actor* giant, Actor* tiny);
    void TinyCalamity_BonusSpeed(Actor* giant);
}