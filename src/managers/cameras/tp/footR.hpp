#pragma once
#include "managers/cameras/tp/foot.hpp"




namespace GTS {
	class FootR : public Foot {
		protected:
			virtual NiPoint3 GetFootPos() override;
	};
}
