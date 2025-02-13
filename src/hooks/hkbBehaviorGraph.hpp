#pragma once
#include "hooks/hooks.hpp"

namespace Hooks
{
	using namespace SKSE;
	using namespace RE;
	class Hook_hkbBehaviorGraph
	{
		public:
			static void Hook();
		private:

			static void Update(hkbBehaviorGraph* a_this, const hkbContext& a_context, float a_timestep);
			static inline REL::Relocation<decltype(Update)> _Update;
	};
}
