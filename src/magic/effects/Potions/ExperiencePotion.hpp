#pragma once
#include "magic/magic.hpp"
// Module that handles Size Potion






namespace GTS {
	class ExperiencePotion : public Magic {
		public:
			using Magic::Magic;
			
			virtual std::string GetName() override;

			virtual void OnStart() override;
	};
}
