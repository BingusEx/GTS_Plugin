#pragma once
#include "magic/magic.hpp"
#include "timer.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;


namespace Gts {
	class Shrink_Poison : public Magic {
		public:
			using Magic::Magic;

			virtual void OnStart() override;

			virtual void OnUpdate() override;

			virtual void OnFinish() override;

			virtual std::string GetName() override;
	};
}
