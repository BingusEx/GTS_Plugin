
#pragma once
/*
 *  Hooks for actor rotation
 */

#include "hooks/hooks.hpp"


using namespace SKSE;

namespace Hooks
{
	class Hook_ActorRotation
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}