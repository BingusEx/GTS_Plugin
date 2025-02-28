#pragma once 
#include "AIData.hpp"
#include "Config/Config.hpp"

namespace GTS {

	class AIManager : public EventListener {
		public:

		[[nodiscard]] static AIManager& GetSingleton() noexcept {
			static AIManager Instance;
			return Instance;
		}

		std::string DebugName() override {
			return "AIManager";
		}


		Timer BeginNewActionTimer = Timer(3.0f);

		void Update() override;
		void Reset() override;
		void ResetActor(Actor* a_Actor) override;

		AIData& GetData(Actor* a_Actor);
		void TryStartAction(Actor* a_Performer) const;

		std::unordered_map<FormID, AIData> AIDataMap;

		SettingsAI& AISettings = Config::GetAI();

	};
}

