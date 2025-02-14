#pragma once

#include "events.hpp"
#include "spring.hpp"

using namespace std;
using namespace SKSE;

using namespace GTS;

namespace GTS {
	float Get_Bone_Movement_Speed(Actor* actor, NodeMovementType type);
	float Get_Bone_Movement_Speed(Actor* giant, DamageSource Source);
};