#pragma once

#include "managers/animation/Utils/CooldownManager.hpp"







namespace GTS
{

	class Animation_Cleavage {
		public:
			static void LaunchCooldownFor(Actor* giant, CooldownSource Source);
			static void RegisterEvents();
			static void RegisterTriggers();
	};
}
