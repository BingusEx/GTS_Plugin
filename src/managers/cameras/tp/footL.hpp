#pragma once
#include "managers/cameras/tp/foot.hpp"
#include "spring.hpp"



namespace GTS {
	class FootL : public Foot {
		protected:
			virtual NiPoint3 GetFootPos() override;
	};
}
