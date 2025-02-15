#pragma once
// Module for the Gts Related code

#include "Utils/Spring.hpp"

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
