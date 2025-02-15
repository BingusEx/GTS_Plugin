#include "managers/cameras/tp/footR.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/highheel.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"


using namespace RE;

namespace {
	const float OFFSET = 0.04f * 70.0f; // About 4cm down
}

namespace GTS {
	NiPoint3 FootR::GetFootPos() {
		const std::string_view rightFootLookup = "NPC R Foot [Rft ]";
		auto player = GetCameraActor();
		if (player) {
			auto rootModel = player->Get3D(false);
			if (rootModel) {
				auto playerTrans = rootModel->world;
				playerTrans.scale = rootModel->parent ? rootModel->parent->world.scale : 1.0f;  // Only do translation/rotation
				auto transform = playerTrans.Invert();
				auto rightFoot = find_node(player, rightFootLookup);
				if (rightFoot) {
					float playerScale = get_visual_scale(player);
					auto rightPosLocal = transform * (rightFoot->world * NiPoint3());
					this->smoothFootPos.target = rightPosLocal;

					this->smoothFootPos.target.z += OFFSET*playerScale;
				}
			}
		}
		return this->smoothFootPos.value;
	}
}
