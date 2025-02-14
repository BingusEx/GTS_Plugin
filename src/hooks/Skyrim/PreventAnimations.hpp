
#pragma once
/*
 *  Hooks for actor rotation
 */

#include "Hooks/Hooks.hpp"

namespace Hooks
{	
	using namespace SKSE;
	using namespace RE;
	using namespace GTS;

	class Hook_PreventAnimations
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}