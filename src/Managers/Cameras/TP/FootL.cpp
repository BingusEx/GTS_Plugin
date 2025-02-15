#include "managers/cameras/tp/footL.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/highheel.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"




namespace {
	const float OFFSET = -0.04f * 70.0f; // About 4cm down
}

namespace GTS {
	NiPoint3 FootL::GetFootPos() {
		const std::string_view leftFootLookup = "NPC L Foot [Lft ]";
		auto player = GetCameraActor();
		if (player) {
			auto rootModel = player->Get3D(false);
			if (rootModel) {
				auto playerTrans = rootModel->world;
				playerTrans.scale = rootModel->parent ? rootModel->parent->world.scale : 1.0f;  // Only do translation/rotation
				auto transform = playerTrans.Invert();
				auto leftFoot = find_node(player, leftFootLookup);
				if (leftFoot) {
					float playerScale = get_visual_scale(player);
					auto leftPosLocal = transform * (leftFoot->world * NiPoint3());
					this->smoothFootPos.target = leftPosLocal;

					this->smoothFootPos.target.z += OFFSET*playerScale;
				}
			}
		}
		return this->smoothFootPos.value;
	}
}
