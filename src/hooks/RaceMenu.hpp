#pragma once
/*
 *  Hooks on various GetScale calls
 */

#include "hooks/hooks.hpp"


using namespace SKSE;

namespace Hooks
{
	class Hook_RaceMenu
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}