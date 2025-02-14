#pragma once
/*
 *  Hooks on various GetScale calls
 */

#include "Hooks/Hooks.hpp"


using namespace SKSE;

namespace Hooks
{
	class Hook_Movement
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}