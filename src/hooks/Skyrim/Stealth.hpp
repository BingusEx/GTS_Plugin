#pragma once
/*
 *  Hooks on various Stealth calls
 */

#include "Hooks/Hooks.hpp"


using namespace SKSE;

namespace Hooks
{
	class Hook_Stealth
	{
		public:
			static void Hook(Trampoline& trampoline);
	};
}