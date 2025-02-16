#pragma once

#include "Managers/Cameras/TP/Foot.hpp"

namespace GTS {
	class FootR : public Foot {
		protected:
			virtual NiPoint3 GetFootPos() override;
	};
}
