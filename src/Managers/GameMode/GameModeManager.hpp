#pragma once
#include "Config/SettingsList.hpp"

namespace GTS {

	enum class BalanceModeInfo {
		SizeGain_Penalty,
		ShrinkRate_Base,
		ShrinkRate_Combat,
	};


	class GameModeManager : public EventListener  {
		public:
			[[nodiscard]] static GameModeManager& GetSingleton() noexcept;
			virtual std::string DebugName() override;

			static void ApplyGameMode(Actor* a_Actor, const SelectedGameMode& a_SelectedGameMode, const float& a_GrowthRate, const float& a_ShrinkRate);
			static void GameMode(Actor* actor);
	};
}
