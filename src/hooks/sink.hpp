#pragma once
/*
 *  Hooks for sinking
 */

#include "hooks/hooks.hpp"


using namespace SKSE;

namespace Hooks
{
	using namespace SKSE;
	using namespace RE;
	class Hook_Sinking
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}
