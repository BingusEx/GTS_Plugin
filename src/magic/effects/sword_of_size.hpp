#pragma once
#include "magic/magic.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;


namespace GTS {
	class SwordOfSize : public Magic {
		public:
			virtual void OnStart() override;
			virtual void OnUpdate() override;
			virtual void OnFinish() override;

			virtual std::string GetName() override;

			SwordOfSize(ActiveEffect* effect);

			private:
				float power = 0.0f;
	};
}
