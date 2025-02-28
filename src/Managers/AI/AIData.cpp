#include "Managers/AI/AIData.hpp"

namespace GTS {

	AIData::AIData(Actor* a_Actor) {

		if (a_Actor) {
			TargetActor = a_Actor->CreateRefHandle();
		}
		else {
			TargetActor = ActorHandle();
		}
	}

}
