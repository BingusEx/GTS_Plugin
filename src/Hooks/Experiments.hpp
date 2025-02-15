#pragma once
/*
 *  Hooks on various GetScale calls
 */

#include "hooks/hooks.hpp"

namespace Hooks
{
	
	
	class Hook_Experiments
	{
		public:
			static void PatchShaking();
			static void Hook(Trampoline& trampoline);
	};
}
