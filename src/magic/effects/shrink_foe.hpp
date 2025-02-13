#pragma once
#include "magic/magic.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;


namespace Gts {
	class ShrinkFoe : public Magic {
		public:
			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;

			ShrinkFoe(ActiveEffect* effect);

		private:
			float power = 0.0f;
			float efficiency = 0.0f;
			float time_mult = 1.0f;
	};
}
