#include "Managers/AI/AIFunctions.hpp"
#include "Managers/Animation/Utils/CooldownManager.hpp"

using namespace GTS;

namespace {

	void DisableEssentialFlag(Actor* actor) {
		if (actor->IsEssential()) {
			actor->GetActorRuntimeData().boolFlags.reset(RE::Actor::BOOL_FLAGS::kEssential); // Else they respawn.
			actor->AsActorState()->actorState1.lifeState = ACTOR_LIFE_STATE::kDead;
			auto data = actor->GetActorBase()->As<TESActorBaseData>();
			if (data) {
				log::info("ActorData exists");
				data->actorData.actorBaseFlags.reset(ACTOR_BASE_DATA::Flag::kEssential);
				data->actorData.actorBaseFlags.reset(ACTOR_BASE_DATA::Flag::kProtected);
			}
		}
	}

	void DisableDeathDialogueTask(Actor* tiny) {
		std::string name = std::format("MuteDeath_{}", tiny->formID);
		ActorHandle tinyRef = tiny->CreateRefHandle();

		TaskManager::RunFor(name, 0.25f, [=](auto& progressData) {
			if (!tinyRef) {
				return false;
			}
			auto actor = tinyRef.get().get();

			if (actor && actor->formID != 0x14) {
				auto process = actor->GetActorRuntimeData().currentProcess;
				if (process) {
					auto high = process->high;
					if (high) {
						high->deathDialogue = false;
						for (auto handles: {0, 1, 2}) {
							auto handle = high->soundHandles[handles];
							if (handle.soundID != BSSoundHandle::kInvalidID) {
								handle.SetVolume(0.0f);
							}
						}
					}
				}
			}
			return true;
		});
	}
}

namespace GTS {

	float GetScareThreshold(Actor* giant) {
		float threshold = 2.5f;
		if (giant->IsSneaking()) { // If we sneak/prone/crawl = make threshold bigger so it's harder to scare actors
			threshold += 0.8f;
		}
		if (IsCrawling(giant)) {
			threshold += 1.45f;
		}
		if (IsProning(giant)) {
			threshold += 1.45f;
		}
		if (giant->AsActorState()->IsWalking()) { // harder to scare if we're approaching slowly
			threshold *= 1.35f;
		}
		if (giant->AsActorState()->IsSprinting()) { // easier to scare
			threshold *= 0.75f;
		}
		return threshold;
	}

	void Task_InitHavokTask(Actor* tiny) {

		double startTime = Time::WorldTimeElapsed();
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		std::string taskname = std::format("EnterRagdoll_{}", tiny->formID);

		TaskManager::RunFor(taskname, 2.0f, [=](auto& update){
			if (!tinyHandle) {
				return false;
			}
			Actor* tinyref = tinyHandle.get().get();
			if (!tinyref) {
				return false;
			}
			if (!tinyref->Is3DLoaded()) {
				return true;
			}
			if (!tinyref->IsDead()) {
				return true;
			}
			double endTime = Time::WorldTimeElapsed();

			if ((endTime - startTime) > 0.05) {
				tinyref->InitHavok(); // Hopefully will fix occasional Ragdoll issues
				return false;
			} 
			return true;
		});
	}

	void SendDeathEvent(Actor* giant, Actor* tiny) {
		auto* eventsource = ScriptEventSourceHolder::GetSingleton();
		if (eventsource) {
			TESObjectREFR* dyingTiny = skyrim_cast<TESObjectREFR*>(tiny);
			TESObjectREFR* killer = skyrim_cast<TESObjectREFR*>(giant);

			if (dyingTiny && killer) {
				ObjectRefHandle dyingRefr = dyingTiny->CreateRefHandle();
				ObjectRefHandle killerRefr = killer->CreateRefHandle();

				if (dyingRefr && killerRefr) {
					TESObjectREFRPtr dying_get = dyingRefr.get();
					TESObjectREFRPtr killer_get = killerRefr.get();
					if (dying_get && killer_get) {
						auto event = TESDeathEvent();
						event.actorDying = dying_get;
						event.actorKiller = killer_get;
						event.dead = true;
						eventsource->SendEvent(&event);
					}
				}
			}
		}
	}

	void KillActor(Actor* giant, Actor* tiny, bool silent) {
		DisableEssentialFlag(tiny); // Prevent Essentials from reappearing
		if (silent) {
			DisableDeathDialogueTask(tiny);
		}

		if (tiny && tiny->Is3DLoaded() && !tiny->IsDead()) {
			StartCombat(tiny, giant);
		}

		float hp = GetMaxAV(tiny, ActorValue::kHealth) * 9.0f;	

		InflictSizeDamage(giant, tiny, hp); // just to make sure

		if (tiny->formID == 0x14) {
			tiny->KillImpl(giant, 1, true, true);
			tiny->SetAlpha(0.0f);
		} 
		SendDeathEvent(giant, tiny);
		Task_InitHavokTask(tiny);
	}

	// butt crush related things

	float GetGrowthCount(Actor* giant) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			return transient->ButtCrushGrowthAmount;
		}
		return 1.0f;
	}

	float GetGrowthLimit(Actor* actor) {
		float limit = 0;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_GrowingDisaster")) {
			limit += 3.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			limit += 4.0f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_LoomingDoom")) {
			limit += 5.0f;
		}
		return limit;
	}

	float GetButtCrushDamage(Actor* actor) {
		float damage = 1.0f;
		if (Runtime::HasPerkTeam(actor, "ButtCrush_KillerBooty")) {
			damage += 0.30f;
		}
		if (Runtime::HasPerkTeam(actor, "ButtCrush_UnstableGrowth")) {
			damage += 0.70f;
		}
		return damage;
	}

	void ModGrowthCount(Actor* giant, float value, bool reset) {
		auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
			transient->ButtCrushGrowthAmount += value;
			if (reset) {
				transient->ButtCrushGrowthAmount = 0.0f;
			}
		}
	}

	void RecordStartButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) { 
			saved_data->buttcrush_start_scale = get_target_scale(giant);
		}
	}

	void SetButtCrushSize(Actor* giant, float value, bool reset) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float scale = game_getactorscale(giant);
			saved_data->buttcrush_max_size += value * scale;

			if (!reset && GetGrowthCount(giant) <= 1) {
				RecordStartButtCrushSize(giant);
			}

			if (reset) {
				float remove_size = saved_data->buttcrush_max_size;
				float start_scale = saved_data->buttcrush_start_scale;

				if (start_scale < get_natural_scale(giant)) { // Protect against going into negatives
					start_scale = get_natural_scale(giant);
				}

				set_target_scale(giant, start_scale); 
				
				saved_data->buttcrush_max_size = 0;
				saved_data->buttcrush_start_scale = 0.0f;
			}
		}
	}

	float GetButtCrushSize(Actor* giant) {
		auto saved_data = Transient::GetSingleton().GetData(giant);
		if (saved_data) {
			float butt_crush_size = std::clamp(saved_data->buttcrush_max_size, 0.0f, 1000000.0f);
			return butt_crush_size;
		}
		return 0.0f;
	}

	void ForceFlee(Actor* giant, Actor* tiny, float duration, bool apply_size_difference) {
		float oldConfidence = GetAV(tiny, ActorValue::kConfidence);

		double Start = Time::WorldTimeElapsed();
		std::string name = std::format("ScareAway_{}", tiny->formID);
		ActorHandle tinyHandle = tiny->CreateRefHandle();
		ActorHandle giantHandle = giant->CreateRefHandle();
		if (apply_size_difference) {
			duration *= GetSizeDifference(giant, tiny, SizeType::VisualScale, false, true);
		}

		SetAV(tiny, ActorValue::kConfidence, 0.0f);

		TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyHandle) {
				return false;
			}
			if (!giantHandle) {
				return false;
			}
			double Finish = Time::WorldTimeElapsed();

			auto tinyRef = tinyHandle.get().get();
			auto giantRef = giantHandle.get().get();

			if (!tinyRef || !tinyRef->Is3DLoaded()) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false;
			}

			if (tinyRef->IsDead()) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false; // To be safe
			}

			ApplyActionCooldown(tinyRef, CooldownSource::Action_ScareOther);

			double timepassed = Finish - Start;
			if (IsMoving(tinyRef)) {
				int FallChance = RandomInt(0, 6000);// Chance to Trip
				if (FallChance <= 2 && !IsRagdolled(tinyRef)) {
					PushActorAway(giantRef, tinyRef, 1.0f);
				}
			}
			
			if (timepassed >= static_cast<float>(duration)) {
				SetAV(tinyRef, ActorValue::kConfidence, oldConfidence);
				return false; // end it
			}
			return true;
		});
	}

	void ScareActors(Actor* giant) {
		auto profiler = Profilers::Profile("ActorUtils: ScareActors");
		if (!Persistent::GetSingleton().actors_panic) {
			return; // Disallow Panic if bool is false.
		}
		for (auto tiny: FindSomeActors("AiActors", 2)) {
			if (tiny != giant && tiny->formID != 0x14 && !IsTeammate(tiny)) {
				if (tiny->IsDead() || IsInSexlabAnim(tiny, giant)) {
					return;
				}
				if (IsBeingHeld(giant, tiny)) {
					return;
				}
				float get_difference = GetSizeDifference(giant, tiny, SizeType::VisualScale, false, true); // Apply HH difference as well
				float sizedifference = std::clamp(get_difference, 0.10f, 12.0f);

				float distancecheck = 128.0f * GetMovementModifier(giant);
				float threshold = GetScareThreshold(giant);

				if (sizedifference >= threshold) {
					NiPoint3 GiantDist = giant->GetPosition();
					NiPoint3 ObserverDist = tiny->GetPosition();
					float distance = (GiantDist - ObserverDist).Length();

					if (distance <= distancecheck * sizedifference) {
						auto combat = tiny->GetActorRuntimeData().combatController;

						tiny->GetActorRuntimeData().currentCombatTarget = giant->CreateRefHandle();
						auto TinyRef = skyrim_cast<TESObjectREFR*>(tiny);

						if (TinyRef) {
							auto GiantRef = skyrim_cast<TESObjectREFR*>(giant);
							if (GiantRef) {
								bool SeeingOther;
								bool IsTrue = tiny->HasLineOfSight(GiantRef, SeeingOther);
								if (IsTrue || distance < (distancecheck/1.5f) * sizedifference) {
									auto cell = tiny->GetParentCell();
									if (cell) {
										if (!combat) {
											tiny->InitiateFlee(TinyRef, true, true, true, cell, TinyRef, 100.0f, 465.0f * sizedifference);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
