#pragma once
// Module that handles footsteps

#include "managers/impact.hpp"
#include "events.hpp"

using namespace std;
using namespace SKSE;


namespace GTS {
	class FootStepManager : public EventListener {
		public:
			[[nodiscard]] static FootStepManager& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void OnImpact(const Impact& impact) override;
			static void PlayLegacySounds(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale);
			static void PlayHighHeelSounds(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale);
			static void PlayNormalSounds(float modifier, NiAVObject* foot, FootEvent foot_kind, float scale);

			static float Volume_Multiply_Function(Actor* actor, FootEvent Kind);

			static void PlayVanillaFootstepSounds(Actor* giant, bool right);
			static void DoStrongSounds(Actor* giant, float animspeed, std::string_view feet);
	};
}
