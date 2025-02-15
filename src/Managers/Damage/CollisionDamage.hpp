#pragma once
#include <vector>
#include <atomic>
#include <unordered_map>

#include <RE/Skyrim.h>







// Module for accurate size-related damage

namespace GTS {

	class CollisionDamage : public EventListener  {
		public:
			[[nodiscard]] static CollisionDamage& GetSingleton() noexcept;

			virtual std::string DebugName() override;

			void DoFootCollision(Actor* actor, float damage, float radius, int random, float bbmult, float crush_threshold, DamageSource Cause, bool right, bool ApplyCooldown, bool ignore_rotation, bool SupportCalamity);
			void DoSizeDamage(Actor* giant, Actor* tiny, float damage, float bbmult, float crush_threshold, int random, DamageSource Cause, bool apply_damage);

			static void CrushCheck(Actor* giant, Actor* tiny, float size_difference, float crush_threshold, DamageSource Cause);
	};
}
