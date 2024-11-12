#include "managers/cameras/tp/footL.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/highheel.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "node.hpp"

using namespace RE;

namespace {
	const float OFFSET = -0.02f * 70.0f; // About 2cm down
}

namespace Gts {
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
				if (leftFoot != nullptr) {
					float playerScale = get_visual_scale(player);
					auto leftPosLocal = transform * (leftFoot->world * NiPoint3());
					this->smoothFootPos.target = leftPosLocal;

					NiPoint3 highheelOffset = HighHeelManager::GetHHOffset(player) * HighHeelManager::GetHHMultiplier(player);
					this->smoothFootPos.target.z += OFFSET*playerScale;
					if (highheelOffset.Length() > 1e-4) {
						this->smoothFootPos.target -= highheelOffset * 1.6f;
					}
				}
			}
		}
		return this->smoothFootPos.value;
	}
}
