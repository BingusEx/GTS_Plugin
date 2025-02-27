#pragma once

// Module that handles AttributeValues

namespace GTS {

	class AttributeManager : public EventListener {
		public:
			[[nodiscard]] static AttributeManager& GetSingleton() noexcept;

			virtual std::string DebugName() override;
			virtual void Update() override;

			static void OverrideSMTBonus(float Value);
			float GetAttributeBonus(Actor* actor, ActorValue av) const;

			static float AlterGetAv(Actor* actor, ActorValue av, float originalValue);
			static float AlterGetBaseAv(Actor* actor, ActorValue av, float originalValue);
			static float AlterSetBaseAv(Actor* actor, ActorValue av, float originalValue);
			static float AlterGetPermenantAv(Actor* actor, ActorValue av, float originalValue);
			static float AlterMovementSpeed(Actor* actor, const NiPoint3& direction);
			static float AlterGetAvMod(float orginal_value, Actor* a_this, RE::ACTOR_VALUE_MODIFIER a_modifier, ActorValue a_value);
		private:
			const SoftPotential speed_adjustment_walk {
				.k = 0.265f, // 0.125
				.n = 1.11f, // 0.86
				.s = 2.0f, // 1.12
				.o = 1.0f,
				.a = 0.0f,  //Default is 0
			};
	};
}
