/*

void RecordSneakingState(Actor* giant, Actor* tiny) {
	bool Crawling = IsCrawling(giant);
	bool Sneaking = giant->IsSneaking();

	tiny->SetGraphVariableBool("GTS_Hug_Sneak_Tny", Sneaking); // Is Sneaking?
	tiny->SetGraphVariableBool("GTS_Hug_Crawl_Tny", Crawling); // Is Crawling?
}

void AI_DoHugs(Actor* pred) {
	if (Persistent::GetSingleton().Hugs_Ai) {
		int rng = RandomInt(0, 7);
		if (rng >= 2) {
			if (CanDoPaired(pred) && !IsSynced(pred) && !IsTransferingTiny(pred)) {
				auto& hugs = HugAnimationController::GetSingleton();
				std::vector<Actor*> preys = hugs.GetHugTargetsInFront(pred, 1);
				for (auto prey : preys) {
					// ^ If Size > 0.92 (minimum) && Size < 2.5 + perk bonus (maximum) threshold basically
					AI_StartHugs(pred, prey);

					AttackManager::PreventAttacks(pred, prey);
				}

				if (preys.empty()) {
					AttackManager::PreventAttacks(pred, nullptr); // Toggle Vanilla Attacks back
				}
			}
		}
	}
}

void AI_StartHugs(Actor* pred, Actor* prey) {
	auto& hugging = HugAnimationController::GetSingleton();
	auto& persist = Persistent::GetSingleton();
	if (!pred->IsInCombat() && persist.vore_combatonly) {
		return;
	}
	if (prey->formID != 0x14 && !IsHostile(pred, prey) && !IsTeammate(pred)) {
		return;
	}
	if (prey->formID == 0x14 && !persist.vore_allowplayervore) {
		return;
	}

	RecordSneakingState(pred, prey); // Needed to determine which hugs to play: sneak or crawl ones (when sneaking)

	HugShrink::GetSingleton().HugActor(pred, prey);

	AnimationManager::StartAnim("Huggies_Try", pred);

	if (pred->IsSneaking()) {
		AnimationManager::StartAnim("Huggies_Try_Victim_S", prey); // GTSBEH_HugAbsorbStart_Sneak_V
	}
	else {
		AnimationManager::StartAnim("Huggies_Try_Victim", prey); //   GTSBEH_HugAbsorbStart_V
	}

	AI_StartHugsTask(pred, prey);
	DisarmActor(prey, false);
}

void AI_StartHugsTask(Actor* giant, Actor* tiny) {
	std::string name = std::format("Huggies_Forced_{}", giant->formID);
	ActorHandle gianthandle = giant->CreateRefHandle();
	ActorHandle tinyhandle = tiny->CreateRefHandle();
	static Timer ActionTimer = Timer(2.5);
	TaskManager::Run(name, [=](auto& progressData) {
		if (!gianthandle) {
			return false;
		}
		if (!tinyhandle) {
			return false;
		}
		auto giantref = gianthandle.get().get();
		auto tinyref = tinyhandle.get().get();

		bool AllyHugged;
		bool IsDead = (tinyref->IsDead() || giantref->IsDead());
		tinyref->GetGraphVariableBool("GTS_IsFollower", AllyHugged);

		if (!HugShrink::GetHuggiesActor(giantref)) {
			if (!AllyHugged && tinyref->formID != 0x14) {
				PushActorAway(giantref, tinyref, 1.0f);
			}
			return false;
		}
		if (ActionTimer.ShouldRunFrame()) {
			int rng = RandomInt(0, 20);
			if (rng < 12) {
				if (!Runtime::HasPerkTeam(giantref, "HugCrush_LovingEmbrace")) {
					rng = 1; // always force crush and always shrink
				}

				if (AI_CanHugCrush(giantref, tinyref, rng)) {
					AnimationManager::StartAnim("Huggies_HugCrush", giantref);
					AnimationManager::StartAnim("Huggies_HugCrush_Victim", tinyref);
				}
				else {
					log::info("HealOrShrink");
					AI_HealOrShrink(giant, tiny, rng);
				}
			}
		}
		if (IsDead) {
			return false;
		}
		return true;
		});
}

bool AI_CanHugCrush(Actor* giant, Actor* tiny, int rng) {
	int crush_rng = RandomInt(0, 4);

	float health = GetHealthPercentage(tiny);
	float HpThreshold = GetHugCrushThreshold(giant, tiny, true);

	bool low_hp = (health <= HpThreshold);
	bool allow_perform = (tiny->formID != 0x14 && IsHostile(giant, tiny)) || (rng <= 1);
	bool Can_HugCrush = (low_hp && allow_perform);

	float stamina = GetStaminaPercentage(giant);
	bool Can_Force = Runtime::HasPerkTeam(giant, "HugCrush_MightyCuddles") && IsHostile(giant, tiny);

	if (Can_Force && crush_rng <= 1 && stamina >= 0.75f) {
		return true;
	}
	if (Can_HugCrush) {
		return true;
	}
	return false;
}

void AI_HealOrShrink(Actor* giant, Actor* tiny, int rng) {
	bool hostile = IsHostile(giant, tiny);
	bool IsCrushing = IsHugCrushing(giant);

	if (!IsCrushing && (hostile || rng <= 1)) { // chance to get drained by follower, it is always 1 without Loving Embrace perk

		float sizedifference = GetSizeDifference(giant, tiny, SizeType::TargetScale, false, true);
		if (!IsHugHealing(giant) && sizedifference >= GetHugShrinkThreshold(giant)) {
			AbortHugAnimation(giant, tiny); // Cancel anim if below shrink threshold
			log::info("Sizediff is > threshold, aborting");
		}
		else {
			AnimationManager::StartAnim("Huggies_Shrink", giant);
			AnimationManager::StartAnim("Huggies_Shrink_Victim", tiny);
		}
	}
	else { // else heal
		StartHealingAnimation(giant, tiny);
	}
}

*/