#pragma once

#include "Managers/Cameras/TP/Foot.hpp"

namespace GTS {
	class FootL : public Foot {
		protected:
			virtual NiPoint3 GetFootPos() override;
	};
}
