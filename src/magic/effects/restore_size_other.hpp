#pragma once
#include "magic/magic.hpp"

// Module that handles footsteps






namespace GTS {
	class RestoreSizeOther : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual std::string GetName() override;
	};
}
