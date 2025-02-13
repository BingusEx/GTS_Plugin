
#pragma once
/*
 *  Hooks for actor rotation
 */

#include "hooks/hooks.hpp"

namespace Hooks
{	
	using namespace SKSE;
	using namespace RE;
	using namespace Gts;

	class Hook_PreventAnimations
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}