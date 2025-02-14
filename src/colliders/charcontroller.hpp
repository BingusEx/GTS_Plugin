#pragma once
// Takes a char controller and extracts its collision objects
#include "Colliders/Common.hpp"

namespace GTS {

	using namespace RE;

	class CharContData : public ColliderData {
		public:
			CharContData(bhkCharacterController* charCont);
	};
}
