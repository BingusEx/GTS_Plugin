#pragma once
#include "Hooks/Hooks.hpp"

namespace Hooks
{
	using namespace SKSE;
	using namespace RE;
	class Hook_MagicTarget
	{
		public:
			static void Hook();
		private:

			static bool AddTargetPlayer(PlayerCharacter* player, MagicTarget::AddTargetData& a_targetData);
			static inline REL::Relocation<decltype(AddTargetPlayer)> _AddTargetPlayer;

			static bool AddTargetCharacter(Character* character, MagicTarget::AddTargetData& a_targetData);
			static inline REL::Relocation<decltype(AddTargetCharacter)> _AddTargetCharacter;
	};
}
