#pragma once
#include "Hooks/Hooks.hpp"

namespace Hooks {

	using namespace RE;
	
	class Hook_BGSImpactManager {
		public:
			static void Hook(Trampoline& trampoline);
			static void Hook();
		private:

			static BSEventNotifyControl ProcessEvent(BGSImpactManager* a_this, const BGSFootstepEvent* a_event, BSTEventSource<BGSFootstepEvent>* a_eventSource);
			static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
	};
}
