#pragma once

namespace GTS {

	class SizeHitEffects : public EventListener  {
		public:
			[[nodiscard]] static SizeHitEffects& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			void ApplyEverything(Actor* attacker, Actor* receiver, float damage);
			void PerformInjuryDebuff(Actor* giant, Actor* tiny, float damage, int random);
	};
}