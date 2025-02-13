#pragma once
#include "magic/magic.hpp"
// Module that handles Size Potion


using namespace std;
using namespace SKSE;


namespace Gts {
	class ExperiencePotion : public Magic {
		public:
			using Magic::Magic;
			
			virtual std::string GetName() override;

			virtual void OnStart() override;
	};
}
