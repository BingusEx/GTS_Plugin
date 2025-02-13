#pragma once

using namespace std;
using namespace SKSE;


namespace Gts
{
	class AnimationUnderStomp {
		public:
			static bool ShouldStompUnder_NPC(Actor* giant, float distance);
            static bool ShouldStompUnder(Actor* giant);

			static void RegisterEvents();

			static void RegisterTriggers();
	};
}