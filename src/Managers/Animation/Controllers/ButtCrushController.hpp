#pragma once

namespace GTS {

	class ButtCrushController : public EventListener  {
		public:
			[[nodiscard]] static ButtCrushController& GetSingleton() noexcept;

			virtual std::string DebugName() override;

			static void ButtCrush_OnCooldownMessage(Actor* giant);
			std::vector<Actor*> GetButtCrushTargets(Actor* pred, std::size_t numberOfPrey);
			bool CanButtCrush(Actor* pred, Actor* prey);
			static void StartButtCrush(Actor* pred, Actor* prey);	

			void AllowMessage(bool allow);
		private:
			bool allow_message = false;
	};
}