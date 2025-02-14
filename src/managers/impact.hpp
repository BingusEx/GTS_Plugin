#pragma once
#include "events.hpp"
// Module that handles footsteps


using namespace std;
using namespace SKSE;


namespace GTS {
	class ImpactManager {
		public:
			[[nodiscard]] static ImpactManager& GetSingleton() noexcept;

			void HookProcessEvent(BGSImpactManager* impact, const BGSFootstepEvent* a_event, BSTEventSource<BGSFootstepEvent>* a_eventSource);
	};
}
