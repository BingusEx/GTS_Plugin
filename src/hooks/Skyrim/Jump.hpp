#pragma once

#include "Hooks/Hooks.hpp"

/*
	Hooks for jumping
*/

namespace Hooks {

	using namespace SKSE;
	using namespace RE;
	class Hook_Jumping {
		public:
			static void Hook(Trampoline& trampoline);
		//private:
			//static float GetScaleJumpHook(TESObjectREFR* a_this);
			//static inline REL::Relocation<decltype(GetScaleJumpHook)> _GetScaleJumpHook;
	};
}
