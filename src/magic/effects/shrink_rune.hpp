#pragma once
#include "magic/magic.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;
using namespace RE;

namespace Gts {
	class ShrinkRune : public Magic {
		public:
			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

			ShrinkRune(ActiveEffect* effect);

		private:
			float power = 0.0f;
			float time_mult = 0.6f;
	};
}