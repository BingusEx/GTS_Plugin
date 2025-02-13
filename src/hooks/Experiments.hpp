#pragma once
/*
 *  Hooks on various GetScale calls
 */

#include "hooks/hooks.hpp"

namespace Hooks
{
	using namespace SKSE;
	using namespace RE;
	class Hook_Experiments
	{
		public:
			static void PatchShaking();
			static void Hook(Trampoline& trampoline);
	};
}
