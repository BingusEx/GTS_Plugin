#pragma once
#include "hooks/hooks.hpp"


namespace Hooks
{
	using namespace RE;
	using namespace Gts;
	class Hook_TESCamera
	{
		public:
			static void Hook();
		private:
			static void Update(TESCamera* a_this);
			static inline REL::Relocation<decltype(Update)> _Update;
	};
}
