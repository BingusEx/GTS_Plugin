#include "colliders/actor.hpp"

using namespace SKSE;
using namespace RE;

namespace GTS {
	
	ActorCollisionData::ActorCollisionData(Actor* actor) : charCont(actor ? actor->GetCharController() : nullptr), ragdoll(actor ? GetRagdoll(actor) : nullptr) {}

	std::vector<ColliderData*> ActorCollisionData::GetChildren() {
		return {
		        &this->charCont,
		        &this->ragdoll
		};
	}
}
