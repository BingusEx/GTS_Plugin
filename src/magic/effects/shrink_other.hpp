#pragma once
#include "magic/magic.hpp"
// Module that handles footsteps






namespace GTS {
	class ShrinkOther : public Magic {
		public:
			using Magic::Magic;

			virtual void OnUpdate() override;

			virtual std::string GetName() override;
	};
}
