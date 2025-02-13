#pragma once
// Takes a char controller and extracts its collision objects
#include "colliders/common.hpp"

using namespace std;
using namespace SKSE;


namespace Gts {

	class CharContData : public ColliderData {
		public:
			CharContData(bhkCharacterController* charCont);
	};
}
