
#pragma once

namespace Hooks {

	using namespace SKSE;

	class Hook_PreventAnimations {
		public:
			static void Hook(Trampoline& trampoline);
	};
}