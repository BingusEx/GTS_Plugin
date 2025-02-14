#pragma once
#include "Hooks/Hooks.hpp"




namespace Hooks
{
	using namespace SKSE;
	using namespace RE;
	class Hook_PlayerCamera
	{
		public:
			static void Hook();
		private:
			static void Update(PlayerCamera* a_this);
			static inline REL::Relocation<decltype(Update)> _Update;
	};
}
