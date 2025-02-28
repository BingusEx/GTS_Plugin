
/*
void AI_DoThighCrush(Actor* giant) {
	if (Persistent::GetSingleton().Thigh_Ai == false) {
		return;
	}
	std::vector<Actor*> tinies = ThighCrushController::GetSingleton().GetThighTargetsInFront(giant, 1);
	if (!tinies.empty()) {
		Actor* tiny = tinies[0];
		if (tiny) {
			ThighCrushController::GetSingleton().StartThighCrush(giant, tiny);
			AttackManager::PreventAttacks(giant, tiny);
		}
	}
	else {
		AttackManager::PreventAttacks(giant, nullptr);
	}
}

void AI_StartThighCrushTask(Actor* giant) {
	std::string name = std::format("ThighCrush_{}", giant->formID);
	ActorHandle gianthandle = giant->CreateRefHandle();
	double Start = Time::WorldTimeElapsed();
	static Timer ActionTimer = Timer(6.0);

	TaskManager::Run(name, [=](auto& progressData) {
		if (!gianthandle) {
			return false;
		}
		Actor* giantref = gianthandle.get().get();
		double Finish = Time::WorldTimeElapsed();

		if (Finish - Start > 0.10) {
			if (!IsThighCrushing(giantref)) {
				return false;
			}

			if (ActionTimer.ShouldRunFrame()) {

				bool ForceAbort = GetAV(giantref, ActorValue::kStamina) <= 2.0f;
				DamageAV(giantref, ActorValue::kStamina, 0.025f);

				if (ForceAbort) {
					AnimationManager::StartAnim("ThighLoopExit", giantref);
					return true;
				}

				std::vector<Actor*> targets = ThighCrushController::GetSingleton().GetThighTargetsInFront(giantref, 1);
				if (targets.empty()) {
					AnimationManager::StartAnim("ThighLoopExit", giantref);
					return true;
				}
				else if (!targets.empty() && !ThighCrushController::GetSingleton().CanThighCrush(giantref, targets[0])) {
					AnimationManager::StartAnim("ThighLoopExit", giantref);
					return true;
				}
				else {
					AnimationManager::StartAnim("ThighLoopAttack", giantref);
					return true;
				}
				return true;
			}
			return true;
		}
		return true;
		});
}

void AI_DoSandwich(Actor* pred) {
	if (!Persistent::GetSingleton().Sandwich_Ai || IsCrawling(pred)) {
		return;
	}
	auto& Sandwiching = ThighSandwichController::GetSingleton();

	std::vector<Actor*> preys = Sandwiching.GetSandwichTargetsInFront(pred, 1);
	for (auto prey : preys) {
		if (CanPerformAnimationOn(pred, prey, false)) { // player check is done inside CanSandwich()
			Sandwiching.StartSandwiching(pred, prey);
			auto node = find_node(pred, "GiantessRune", false);
			if (node) {
				node->local.scale = 0.01f;
				update_node(node);
			}

			AttackManager::PreventAttacks(pred, prey);
		}
	}

	if (preys.empty()) {
		AttackManager::PreventAttacks(pred, nullptr); // Toggle Vanilla Attacks back
	}
}
*/
