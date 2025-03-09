#pragma once

#include "Magic/Magic.hpp"

namespace GTS {

	class ShrinkPotion : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

	};
}
