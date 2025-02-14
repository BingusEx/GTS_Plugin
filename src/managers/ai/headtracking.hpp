#pragma once
// Module for the Gts Related code
#include <vector>
#include <atomic>
#include <unordered_map>

#include <RE/Skyrim.h>

#include "spring.hpp"
#include "events.hpp"
#include "node.hpp"

using namespace std;


namespace GTS {

	struct HeadtrackingData {
		Spring spineSmooth = Spring(0.0f, 0.70f);
		Spring casterSmooth = Spring(0.0f, 1.0f);
	};

	class Headtracking : public EventListener  {
		public:
			[[nodiscard]] static Headtracking& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void Update() override;

			void SpineUpdate(Actor* me);
		// void FixNPCHeadtracking(Actor* me);
		// void FixPlayerHeadtracking(Actor* me);
		protected:
			std::unordered_map<FormID, HeadtrackingData> data;
	};
}
