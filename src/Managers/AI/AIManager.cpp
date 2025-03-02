#include "Managers/AI/AIManager.hpp"

#include "Hug/HugAI.hpp"

#include "Managers/AI/Vore/VoreAI.hpp"
#include "Managers/AI/Thigh/ThighCrushAI.hpp"
#include "Managers/AI/ButtCrush/ButtCrushAI.hpp"
#include "Managers/AI/Thigh/ThighSandwichAI.hpp"
#include "Managers/AI/StompKick/StompKickSwipeAI.hpp"

using namespace GTS;

namespace {

	enum class ActionType : uint8_t {
		kVore,
		kStomps,
		kKicks,
		kThighS,
		kThighC,
		kButt,
		kHug,
		kGrab,
		kNone,
		kTotal
	};

	// Check if an actor is a valid GTS Action initiator/performer.
	inline bool ValidPerformer(Actor* a_Actor, const bool a_CombatOnly) {

		if (a_Actor->formID == 0x14) {
			return false;
		}

		//Is "Female?"
		if (IsFemale(a_Actor, true)) {

			const bool HasHP = GetAV(a_Actor, ActorValue::kHealth) > 0;
			const bool IsVisible = a_Actor->GetAlpha() > 0.0f;
			const bool IsInNormalState = a_Actor->AsActorState()->GetSitSleepState() == SIT_SLEEP_STATE::kNormal;

			//Is In combat or do we allow ai outside of combat?
			if ((a_Actor->IsInCombat() || !a_CombatOnly) && !IsGtsBusy(a_Actor) && HasHP && IsVisible && IsInNormalState) {

				//Follower Check
				if (IsTeammate(a_Actor)) {
					return true;
				}

				//Size effects for all enabled makes other npc's valid too.
				if (EffectsForEveryone(a_Actor)) {
					return true;
				}
			}
		}
		return false;
	}

	// Check if an actor is a valid action victim
	inline bool ValidPrey(Actor* a_Prey, const bool a_AllowPlayer, const bool a_AllowTeamMates, const bool a_AllowEssential) {

		if (IsFlying(a_Prey)) {
			return false;
		}

		if (!IsGtsBusy(a_Prey) && a_Prey->GetAlpha() > 0.0f) {

			//If not a teammate and they are essential but we allow essentials
			if (a_Prey->formID == 0x14) {
				if (a_AllowPlayer) {
					return true;
				}
				return false;
			}

			if (IsTeammate(a_Prey)) {
				if (a_AllowTeamMates) {
					return true;
				}
				return false;
			}

			if (a_Prey->IsEssential()) {
				if (a_AllowEssential) {
					return true;
				}

				return false;
			}
			return true;
		}
		return false;
	}

	//Get a list of valid action initiators/performers
	std::vector<RE::Actor*> FindValidPerformers() {

		std::vector<Actor*> ValidPerformers = {};

		const bool CombatOnly = Config::GetAI().bCombatOnly;

		//Get a list of valid perfomer actors, aka actors that are elidgible to start an action.
		for (auto Target : find_actors()) {
			//Skip Nullptr actors
			if (!Target) continue;

			if (ValidPerformer(Target, CombatOnly)) {
				ValidPerformers.push_back(Target);
			}

		}

		return ValidPerformers;

	}

	//Get a list of valid action victims
	std::vector<RE::Actor*> FindValidPrey(Actor* a_Performer) {

		std::vector<Actor*> ValidVictims = {};

		const auto& AISettings = Config::GetAI();
		const auto& GeneralSettings = Config::GetGeneral();

		const bool AllowPlayer = AISettings.bAllowPlayer;
		const bool AllowFollowers = AISettings.bAllowFollowers;
		const bool AllowEssential = !GeneralSettings.bProtectEssentials;

		//Get a list of valid perfomer actors, aka actors that are elidgible to start an action.
		for (auto Target : find_actors()) {
			//Skip Nullptr actors and the performer
			if (!Target || a_Performer == Target) continue;

			if (ValidPrey(Target, AllowPlayer, AllowFollowers, AllowEssential)) {
				ValidVictims.push_back(Target);
			}

		}

		return ValidVictims;

	}

	ActionType CalculateProbability(const std::map<ActionType, int>& a_ValidActionMap) {

		if (a_ValidActionMap.empty()) return ActionType::kNone;

		try {
			std::array<int, static_cast<int>(ActionType::kTotal)> ProbabiltyList = { 0 };

			for (auto Action : a_ValidActionMap) {
				ProbabiltyList[static_cast<int>(Action.first)] = Action.second;
			}

			ProbabiltyList[static_cast<int>(ActionType::kNone)] = 100;

			return static_cast<ActionType>(RandomIntWeighted(ProbabiltyList));
		}
		catch (exception& e) {
			logger::warn("CalculateProbability Exception: {}", e.what());
			return ActionType::kNone;
		}
	}
}

namespace GTS {

	void AIManager::Update() {

		std::ignore = Profilers::Profile("AIManager: Update");

		if (!Plugin::Live()) {
			return;
		}

		BeginNewActionTimer.UpdateDelta(AISettings.fMasterTimer);

		if (!AISettings.bEnableActionAI) return;

		if (BeginNewActionTimer.ShouldRun()) {

			logger::trace("AIManager Update");

			const auto& PerformerList = FindValidPerformers();
			if (!PerformerList.empty()) {

				logger::trace("AIManager Found Performers");

				//Pick random
				int idx = RandomInt(0, static_cast<int>(PerformerList.size()) - 1);
				Actor* Performer = PerformerList.at(idx);
				TryStartAction(Performer);
			}
		}
	}

	void AIManager::TryStartAction(Actor* a_Performer) const {

		//Actor* container from each filter result.
		std::vector<Actor*> CanVore = {};
		std::vector<Actor*> CanStompKickSwipe = {};
		std::vector<Actor*> CanThighSandwich = {};
		std::vector<Actor*> CanThighCrush = {};
		std::vector<Actor*> CanButtCrush = {};
		std::vector<Actor*> CanHug = {};
		std::vector<Actor*> CanGrab = {};

		std::map<ActionType, int> StartableActions;

		const auto& PreyList = FindValidPrey(a_Performer);
		if (PreyList.empty()) {
			return;
		}

		//----------- VORE

		if (AISettings.Vore.bEnableAction) {
			CanVore = VoreAI_FilterList(a_Performer, PreyList);
			if (!CanVore.empty()) {
				StartableActions.emplace(ActionType::kVore, static_cast<int>(AISettings.Vore.fProbability));
			}
		}

		//----------- STOMP
		//----------- KICK/SWIPE

		if (AISettings.Stomp.bEnableAction || AISettings.KickSwipe.bEnableAction) {
			CanStompKickSwipe = StompKickSwipeAI_FilterList(a_Performer, PreyList);

			if (AISettings.Stomp.bEnableAction) {
				if (!CanStompKickSwipe.empty()) {
					StartableActions.emplace(ActionType::kStomps, static_cast<int>(AISettings.Stomp.fProbability));
				}
			}

			if (AISettings.KickSwipe.bEnableAction) {
				if (!CanStompKickSwipe.empty()) {
					StartableActions.emplace(ActionType::kKicks, static_cast<int>(AISettings.KickSwipe.fProbability));
				}
			}
		}

		//----------- THIGH SANDWICH

		if (AISettings.ThighSandwich.bEnableAction) {
			CanThighSandwich = ThighSandwichAI_FilterList(a_Performer, PreyList);
			if (!CanThighSandwich.empty()) {
				StartableActions.emplace(ActionType::kThighS, static_cast<int>(AISettings.ThighSandwich.fProbability));
			}
		}

		//----------- THIGH CRUSH

		if (AISettings.ThighCrush.bEnableAction) {
			CanThighCrush = ThighCrushAI_FilterList(a_Performer, PreyList);
			if (!CanThighCrush.empty()) {
				StartableActions.emplace(ActionType::kThighC, static_cast<int>(AISettings.ThighCrush.fProbability));
			}
		}

		//----------- BUTT CRUSH

		if (AISettings.ButtCrush.bEnableAction) {
			CanButtCrush = ButtCrushAI_FilterList(a_Performer, PreyList);
			if (!CanButtCrush.empty()) {
				StartableActions.emplace(ActionType::kButt, static_cast<int>(AISettings.ThighCrush.fProbability));
			}
		}

		//----------- HUGS

		if (AISettings.Hugs.bEnableAction) {
			CanHug = HugAI_FilterList(a_Performer, PreyList);
			if (!CanHug.empty()) {
				StartableActions.emplace(ActionType::kHug, static_cast<int>(AISettings.Hugs.fProbability));
			}
		}

		//----------- GRAB

		if (AISettings.Grab.bEnableAction) {
			//CanGrab;
			if (!CanGrab.empty()) {
				StartableActions.emplace(ActionType::kGrab, static_cast<int>(AISettings.Grab.fProbability));
			}
		}

		switch (CalculateProbability(StartableActions)) {

			case ActionType::kVore: {

				logger::trace("AI Starting kVore Action");

				if (!CanVore.empty()) {
					VoreAI_StartVore(a_Performer, CanVore);
				}

				return;
			}
			case ActionType::kStomps: {

				logger::trace("AI Starting kStomps Action");

				if (!CanStompKickSwipe.empty()) {
					StompAI_Start(a_Performer, CanStompKickSwipe.front());
				}

				return;
			}
			case ActionType::kKicks: {
				logger::trace("AI Starting kKicks Action");

				if (!CanStompKickSwipe.empty()) {
					KickSwipeAI_Start(a_Performer);
				}

				return;
			}
			case ActionType::kThighS: {
				logger::trace("AI Starting kThighS Action");

				if (!CanThighSandwich.empty()) {
					ThighSandwichAI_Start(a_Performer, CanThighSandwich);
				}

				return;
			}
			case ActionType::kThighC: {
				logger::trace("AI Starting kThighC Action");

				if (!CanThighCrush.empty()) {
					ThighCrushAI_Start(a_Performer);
				}

				return;
			}
			case ActionType::kButt: {
				logger::trace("AI Starting kButt Action");

				if (!CanButtCrush.empty()) {
					ButtCrushAI_Start(a_Performer, CanButtCrush.front());
				}

				return;
			}
			case ActionType::kHug: {
				logger::trace("AI Starting kHug Action");

				if (!CanHug.empty()) {
					HugAI_Start(a_Performer, CanHug.front());
				}

				return;
			}
			case ActionType::kGrab: {
				logger::trace("AI Starting kGrab Action");
				return;
			}

			default:{}
		}

	}

	void AIManager::Reset() {
		this->AIDataMap.clear();
	}

	void AIManager::ResetActor(Actor* a_Actor) {
		this->AIDataMap.erase(a_Actor->formID);
	}

	AIData& AIManager::GetData(Actor* a_Actor) {
		// Create it now if not there yet
		this->AIDataMap.try_emplace(a_Actor->formID, a_Actor);
		return this->AIDataMap.at(a_Actor->formID);
	}

}
