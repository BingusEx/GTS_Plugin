#include "Managers/AttackManager.hpp"

#include "Config/Config.hpp"

using namespace GTS;

namespace {

	bool IsHumanoid(Actor* giant) {
		const bool Human = Runtime::HasKeyword(giant, "ActorTypeNPC");
		return Human;
	}

	void DisableAttacks_Melee(Actor* a_Giant, float a_SizeDiff, float a_Threshold, bool a_Reset) {

		if (a_Reset) {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kAttackingDisabled);
			return;
		}

		const float Random = RandomFloat(0.0f, 20.0f);
		if (a_SizeDiff >= 2.5f && Random <= a_Threshold) {
			a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kAttackingDisabled);
		}
		else {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kAttackingDisabled);
		}
	}
	void DisableAttacks_Magic(Actor* a_Giant, float a_SizeDiff, float a_Threshold, bool a_Reset) {

		if (a_Reset) {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kCastingDisabled);
			return;
		}

		const float Random = RandomFloat(0.0f, 20.0f);
		if (a_SizeDiff >= 2.5f && Random <= a_Threshold) {
			a_Giant->GetActorRuntimeData().boolFlags.set(Actor::BOOL_FLAGS::kCastingDisabled);
		}
		else {
			a_Giant->GetActorRuntimeData().boolFlags.reset(Actor::BOOL_FLAGS::kCastingDisabled);
		}
	}

    void PreventAttacking() { // Old LOOP version of it
		static Timer CheckTimer = Timer(0.50);
        auto profiler = Profilers::Profile("AttackManager: PreventAttacking");
		if (CheckTimer.ShouldRunFrame()) {
			for (auto giant: find_actors()) {
				if (giant) {
					if (giant->formID != 0x14 && (IsTeammate(giant) || EffectsForEveryone(giant))) {
						auto receiverRef = giant->GetActorRuntimeData().currentCombatTarget;
						if (receiverRef && IsHumanoid(giant)) {
							auto receiver = receiverRef.get().get();
							if (receiver) {
								log::info("Victim found: {}", receiver->GetDisplayFullName());
								float SizeDiff = GetSizeDifference(giant, receiver, SizeType::VisualScale, true, false);
								float random = RandomFloat(0.0f, 20.0f);
								float threshold = 2.5f * (SizeDiff - 2.5f);
								
								float distance = (giant->GetPosition() - receiver->GetPosition()).Length();
								distance /= get_visual_scale(giant);

								if (distance <= 50.0f) {
									DisableAttacks_Melee(giant, SizeDiff, threshold, false);
									DisableAttacks_Magic(giant, SizeDiff, threshold, false);
									//log::info("Blocking attack for {}", giant->GetDisplayFullName());
								} else {
									DisableAttacks_Melee(giant, 0.0f, 0.0f, true);
									DisableAttacks_Magic(giant, 0.0f, 0.0f, true);
									//log::info("Blocking attack for {} is reset", giant->GetDisplayFullName());
								}
							}
						}
					}
				}
			}
		}
	}
}

namespace GTS {

    AttackManager& AttackManager::GetSingleton() noexcept {
        static AttackManager instance;

        static std::atomic_bool initialized;
        static std::latch latch(1);
        if (!initialized.exchange(true)) {
            latch.count_down();
        }
        latch.wait();

        return instance;
    }

    std::string AttackManager::DebugName() {
        return "::AttackManager";
    }

	void AttackManager::PreventAttacks(Actor* a_Giant, Actor* a_Tiny) {

		if (a_Giant && a_Giant->formID != 0x14 && IsHumanoid(a_Giant)) {

			//If disabled in settings each call to this should always enable instead.
			if (!Config::GetAI().bDisableAttacks) {
				DisableAttacks_Melee(a_Giant, 0.0f, 0.0f, true);
				DisableAttacks_Magic(a_Giant, 0.0f, 0.0f, true);
				return;
			}

			if (a_Tiny) {

				const float SizeDiff = GetSizeDifference(a_Giant, a_Tiny, SizeType::VisualScale, true, false);
				const float Threshold = 2.5f * (SizeDiff - 2.5f);
				DisableAttacks_Melee(a_Giant, SizeDiff, Threshold, false);
				DisableAttacks_Magic(a_Giant, SizeDiff, Threshold, false);

			}
			// If Tiny is nullptr, we count it as 'enable attacks back'
			else {

				DisableAttacks_Melee(a_Giant, 0.0f, 0.0f, true);
				DisableAttacks_Magic(a_Giant, 0.0f, 0.0f, true);

			}
		}
	}
}