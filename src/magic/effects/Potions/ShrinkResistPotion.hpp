#pragma once
#include "magic/magic.hpp"

// Module that handles footsteps


using namespace std;
using namespace SKSE;


namespace GTS {
	class ShrinkResistPotion : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

			ShrinkResistPotion(ActiveEffect* effect);

		private:
			float Resistance = 0.0f;
	};
}