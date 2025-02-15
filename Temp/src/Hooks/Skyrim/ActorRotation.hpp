
#pragma once
/*
  Hooks for actor rotation
*/

#include "Hooks/Hooks.hpp"

namespace Hooks {
	
	class Hook_ActorRotation {
		public:
			static void Hook(Trampoline& trampoline);
	};
}