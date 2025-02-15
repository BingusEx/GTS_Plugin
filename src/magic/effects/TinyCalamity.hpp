#pragma once
#include "magic/magic.hpp"
// Module that handles footsteps






namespace GTS {
	class TinyCalamity : public Magic {
		public:

			using Magic::Magic;

			virtual void OnStart() override;
			virtual void OnUpdate() override;
			virtual void OnFinish() override;

			virtual std::string GetName() override;

	};
}
