
#include "Magic/Effects/Poisons/Poison_Of_Shrinking.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"

namespace GTS {

	std::string Shrink_Poison::GetName() {
		return "Shrink_Poison";
	}

	void Shrink_Poison::OnStart() {
		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		Rumbling::Once("Shrink_Poison", target, 2.0f, 0.05f);

		float Volume = std::clamp(get_visual_scale(target) * 0.10f, 0.10f, 1.0f);
		Runtime::PlaySound("GTSSoundShrink", target, Volume, 1.0f);
	}

	void Shrink_Poison::OnUpdate() {

		constexpr float BASE_POWER = 0.004000f;

		auto caster = GetCaster();
		if (!caster) {
			return;
		}
		auto target = GetTarget();
		if (!target) {
			return;
		}

		float AlchemyLevel = std::clamp(caster->AsActorValueOwner()->GetActorValue(ActorValue::kAlchemy)/100.0f + 1.0f, 1.0f, 2.0f);
		Rumbling::Once("Shrink_Poison", target, 0.4f, 0.05f);
		float powercap = std::clamp(get_visual_scale(target), 0.85f, 1.10f);
		float Power = BASE_POWER * powercap * AlchemyLevel;

		ShrinkActor(target, Power, 0.0f);
		if (get_visual_scale(target) < 0.08f/GetSizeFromBoundingBox(target) && ShrinkToNothingManager::CanShrink(caster, target)) {
			ReportDeath(caster, target, DamageSource::Explode);
			ShrinkToNothingManager::Shrink(caster, target);
		}
	}

	void Shrink_Poison::OnFinish() {
	}
}
