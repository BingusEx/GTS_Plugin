#pragma once
#include <vector>
#include <atomic>
#include <unordered_map>

#include <RE/Skyrim.h>




using namespace std;
using namespace SKSE;

// Module for accurate size-related damage

namespace GTS {
	class LaunchActor : public EventListener  {
		public:
			[[nodiscard]] static LaunchActor& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			static void ApplyLaunchTo(Actor* giant, Actor* tiny, float force, float launch_power);
			void ApplyLaunch_At(Actor* giant, float radius, float power, FootEvent kind);

			void LaunchAtNode(Actor* giant, float radius, float power, std::string_view node);
			void LaunchAtNode(Actor* giant, float radius, float power, NiAVObject* node);

			void LaunchAtCustomNode(Actor* giant, float radius, float min_radius, float power, NiAVObject* node);

			void LaunchAtFoot(Actor* giant, float radius, float power, bool right_foot);
	};
}
