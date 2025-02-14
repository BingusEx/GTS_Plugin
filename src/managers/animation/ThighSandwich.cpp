// Animation: Stomp
//  - Stages
/*
   GTSSandwich_EnterAnim           // Animation was just fired
   GTSSandwich_MoveBody_start      // Rumble for entire body
   GTSSandwich_EnableRune          // Trigger the rune
   GTSSandwich_SitStart            // When sit start happens
   GTSSandwich_MoveBody_end        // Stop them (When body pretty much enters 'idle' state)
   GTSSandwich_MoveLL_start        // Left leg starts to move in space (When starting to crush)
   GTSSandwich_ThighImpact         // When Both thighs are supposed to deal damage to an actor (When 'Sandwiching') happens
   GTSSandwich_MoveLL_end          // Left leg isn't moving anymore (These 2 should be repeated)
   GTSSandwich_ThighLoop_Enter     // Enter Thigh Idle Loop

   GTSSandwich_ThighAttack_start   // When we trigger Thigh Attack

   GTSSandwich_ThighLoop_Exit      // Exit thigh idle loop
   GTSSandwich_DisableRune         // Remove Rune
   GTSSandwich_DropDown            // When actor starts to 'jump off' from Rune
   GTSSandwich_FootImpact          // When both feet collide with the ground
   GTSSandwich_ExitAnim            // Animation is over

   GTSBEH_ThighSandwich_Start
   GTSBEH_ThighSandwich_Attack
   GTSBEH_ThighSandwich_ExitLoop

   //AnimObjectA = Tiny
   //AnimObjectB = rune
 */
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/ThighSandwichController.hpp"
#include "managers/animation/ThighSandwich.hpp"
#include "managers/damage/TinyCalamity.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/audio/GoreAudio.hpp"
#include "managers/cameras/camutil.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/ai/aifunctions.hpp"
#include "managers/audio/footstep.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "utils/InputConditions.hpp"
#include "magic/effects/common.hpp"
#include "managers/explosion.hpp"
#include "utils/actorUtils.hpp"
#include "managers/tremor.hpp"
#include "managers/Rumble.hpp"
#include "data/persistent.hpp"
#include "ActionSettings.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "events.hpp"
#include "spring.hpp"
#include "node.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace GTS;

namespace {
	const std::string_view RNode = "NPC R Foot [Rft ]";
	const std::string_view LNode = "NPC L Foot [Lft ]";

	const std::vector<std::string_view> BODY_NODES = { // used for body rumble
		"NPC COM [COM ]",
		"NPC L Foot [Lft ]",
		"NPC R Foot [Rft ]",
		"NPC L Toe0 [LToe]",
		"NPC R Toe0 [RToe]",
		"NPC L Calf [LClf]",
		"NPC R Calf [RClf]",
		"NPC L PreRearCalf",
		"NPC R PreRearCalf",
		"NPC L FrontThigh",
		"NPC R FrontThigh",
		"NPC R RearCalf [RrClf]",
		"NPC L RearCalf [RrClf]",
	};

	const std::vector<std::string_view> L_LEG_NODES = {
		"NPC L Foot [Lft ]",
		"NPC L Toe0 [LToe]",
		"NPC L Calf [LClf]",
		"NPC L PreRearCalf",
		"NPC L FrontThigh",
		"NPC L RearCalf [RrClf]",
	};



	void AllowToBeCrushed(Actor* actor, bool toggle) {
		auto transient = Transient::GetSingleton().GetData(actor);
		if (transient) {
			transient->can_be_crushed = toggle;
		}
	}

	void DoThighDamage(Actor* giant, Actor* tiny, float animSpeed, float mult, float sizemult) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(giant);
		auto& sizemanager = SizeManager::GetSingleton();

		if (tiny && tiny->Is3DLoaded()) {
			float sizedifference = get_visual_scale(giant)/ (get_visual_scale(tiny) * GetSizeFromBoundingBox(tiny));
			float additionaldamage = 1.0f + sizemanager.GetSizeVulnerability(tiny); // Get size damage debuff from enemy
			float normaldamage = std::clamp(sizemanager.GetSizeAttribute(giant, SizeAttribute::Normal), 1.0f, 999.0f);
			float damage = Damage_ThighSandwich_Impact * sizedifference * animSpeed * mult * normaldamage * GetPerkBonus_Thighs(giant);
			if (HasSMT(giant)) {
				damage *= 1.5f;
			}

			if (CanDoDamage(giant, tiny, false)) {
				InflictSizeDamage(giant, tiny, damage);
			}

			float experience = std::clamp(damage/200, 0.0f, 0.20f);
			ModSizeExperience(giant, experience);

			float hp = GetAV(tiny, ActorValue::kHealth);
			if (damage > hp || hp <= 0) {
				ModSizeExperience_Crush(giant, tiny, true);
				
				CrushManager::GetSingleton().Crush(giant, tiny);
				
				PrintDeathSource(giant, tiny, DamageSource::ThighSandwiched);
				AdvanceQuestProgression(giant, tiny, QuestStage::HandCrush, 1.0f, false);
				auto node = find_node(giant, "NPC R FrontThigh");
				
				PlayCrushSound(giant, node, false, false);

				sandwichdata.Remove(tiny);
			}
		}
	}

	void StartBodyRumble(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: L_LEG_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StopBodyRumble(std::string_view tag, Actor& actor) {
		for (auto& node_name: L_LEG_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void StartLeftLegRumbling(std::string_view tag, Actor& actor, float power, float halflife) {
		for (auto& node_name: L_LEG_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Start(rumbleName, &actor, power,  halflife, node_name);
		}
	}

	void StopLeftLegRumbling(std::string_view tag, Actor& actor) {
		for (auto& node_name: L_LEG_NODES) {
			std::string rumbleName = std::format("{}{}", tag, node_name);
			Rumbling::Stop(rumbleName, &actor);
		}
	}

	void GTSSandwich_EnterAnim(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.EnableSuffocate(false);
	}
	void GTSSandwich_MoveBody_start(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		for (auto tiny: sandwichdata.GetActors()) {
			AllowToBeCrushed(tiny, false);
			SetBeingHeld(tiny, true);
			sandwichdata.MoveActors(true);
			DisableCollisions(tiny, &data.giant);
		}
		StartBodyRumble("BodyRumble", data.giant, 0.5f, 0.25f);
	}
	void GTSSandwich_EnableRune(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		auto& sizemanager = SizeManager::GetSingleton();
		ManageCamera(&data.giant, true, CameraTracking::Thigh_Sandwich); // Focus camera on AnimObjectA
		sandwichdata.EnableRuneTask(&data.giant, false); // Start Growing the Rune
	}
	void GTSSandwich_SitStart(AnimationEventData& data) {
		DrainStamina(&data.giant, "StaminaDrain_Sandwich_Idle", "KillerThighs", true, 0.25f);
	}
	void GTSSandwich_MoveBody_end(AnimationEventData& data) {
		StopBodyRumble("BodyRumble", data.giant);
	}
	void GTSSandwich_MoveLL_start(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = true;
		data.animSpeed = 1.66f;
		if (data.giant.formID != 0x14) {
			data.animSpeed = 1.66f + GetRandomBoost();
		}
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.EnableSuffocate(false);
		StartLeftLegRumbling("LLSandwich", data.giant, 0.10f, 0.12f);
		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", true, 1.0f);
	}

	void GTSSandwich_MoveLL_start_H(AnimationEventData& data) {
		data.stage = 1;
		data.canEditAnimSpeed = true;
		data.animSpeed = 1.66f;
		if (data.giant.formID != 0x14) {
			data.animSpeed = 1.66f + GetRandomBoost();
		}
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		sandwichdata.EnableSuffocate(false);
		StartLeftLegRumbling("LLSandwichHeavy", data.giant, 0.15f, 0.15f);
		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", true, 2.5f);
	}

	void GTSSandwich_ThighImpact(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		Runtime::PlaySoundAtNode("ThighSandwichImpact", &data.giant, 1.0f, 1.0f, "AnimObjectB");
		sandwichdata.EnableSuffocate(true);

		
		for (auto tiny: sandwichdata.GetActors()) {
			DoThighDamage(&data.giant, tiny, data.animSpeed, 1.0f, 1.0f);
			tiny->NotifyAnimationGraph("ragdoll");
			AllowToBeCrushed(tiny, true);
		}
		
		Rumbling::Once("ThighImpact", &data.giant, Rumble_ThighSandwich_ThighImpact, 0.15f, "AnimObjectA", 0.0f);
		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", false, 1.0f);
	}

	void GTSSandwich_ThighImpact_H(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		Runtime::PlaySoundAtNode("ThighSandwichImpact", &data.giant, 1.2f, 1.0f, "AnimObjectA");
		sandwichdata.EnableSuffocate(true);
		
		for (auto tiny: sandwichdata.GetActors()) {
			DoThighDamage(&data.giant, tiny, data.animSpeed, 2.2f, 0.75f);
			Attacked(tiny, &data.giant);
			tiny->NotifyAnimationGraph("ragdoll");
			AllowToBeCrushed(tiny, true);
		}
		
		Rumbling::Once("ThighImpact", &data.giant, Rumble_ThighSandwich_ThighImpact_Heavy, 0.15f, "AnimObjectA", 0.0f);
		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", false, 2.5f);
	}

	void GTSSandwich_MoveLL_end(AnimationEventData& data) {
		data.canEditAnimSpeed = false;
		data.animSpeed = 1.0f;
		StopLeftLegRumbling("LLSandwich", data.giant);
	}

	void GTSSandwich_MoveLL_end_H(AnimationEventData& data) {
		data.canEditAnimSpeed = false;
		data.animSpeed = 1.0f;
		StopLeftLegRumbling("LLSandwichHeavy", data.giant);
	}

	void GTSSandwich_ThighLoop_Enter(AnimationEventData& data) {
	}

	void GTSSandwich_ThighLoop_Exit(AnimationEventData& data) {
		auto& sizemanager = SizeManager::GetSingleton();
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		ManageCamera(&data.giant, false, CameraTracking::Thigh_Sandwich);
		sandwichdata.EnableSuffocate(false);
		sandwichdata.EnableRuneTask(&data.giant, true); // Launch Rune Shrinking
		for (auto tiny: sandwichdata.GetActors()) {
			SetBeingHeld(tiny, false);
			PushActorAway(&data.giant, tiny, 1.0f);
			EnableCollisions(tiny);
		}
		sandwichdata.MoveActors(false);

		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", false, 2.5f);
	}

	void GTSSandwich_ThighAttack_start(AnimationEventData& data) {
	}

	void GTSSandwich_DisableRune(AnimationEventData& data) {

	}

	void GTSSandwich_DropDown(AnimationEventData& data) {
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		for (auto tiny: sandwichdata.GetActors()) {
			AllowToBeCrushed(tiny, true);
		}
		
		sandwichdata.ReleaseAll();

		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", false, 2.5f);
		DrainStamina(&data.giant, "StaminaDrain_Sandwich_Idle", "KillerThighs", false, 0.25f);
	}

	void GTSSandwich_ExitAnim(AnimationEventData& data) {
		auto& sizemanager = SizeManager::GetSingleton();
		auto& sandwichdata = ThighSandwichController::GetSingleton().GetSandwichingData(&data.giant);
		for (auto tiny: sandwichdata.GetActors()) {
			SetBeingHeld(tiny, false);
			EnableCollisions(tiny);
		}
		DrainStamina(&data.giant, "StaminaDrain_Sandwich", "KillerThighs", false, 2.5f);
		ManageCamera(&data.giant, false, CameraTracking::Thigh_Sandwich); // Allow sandwich repeat
	}

	void GTSSandwich_FootImpact(AnimationEventData& data) {
		float perk = GetPerkBonus_Thighs(&data.giant);
		DoFootstepSound(&data.giant, 1.05f, FootEvent::Right, RNode);
		DoFootstepSound(&data.giant, 1.05f, FootEvent::Left, LNode);
		DoDustExplosion(&data.giant, 2.0f, FootEvent::Right, RNode);
		DoDustExplosion(&data.giant, 2.0f, FootEvent::Left, LNode);
		DoDamageEffect(&data.giant, Damage_ThighSandwich_FallDownImpact * perk, Radius_ThighSandwich_FootFallDown, 10, 0.20f, FootEvent::Right, 1.0f, DamageSource::CrushedRight);
		DoDamageEffect(&data.giant, Damage_ThighSandwich_FallDownImpact * perk, Radius_ThighSandwich_FootFallDown, 10, 0.20f, FootEvent::Left, 1.0f, DamageSource::CrushedLeft);

		DoLaunch(&data.giant, 0.85f * perk, 3.2f, FootEvent::Right);
		DoLaunch(&data.giant, 0.85f * perk, 3.2f, FootEvent::Left);

		
		float shake_power = Rumble_ThighSandwich_DropDown/2 * GetHighHeelsBonusDamage(&data.giant, true);

		if (HasSMT(&data.giant)) {
			shake_power *= 2.0f;
		}

		Rumbling::Once("ThighDropDown_R", &data.giant, shake_power, 0.10f, RNode, 0.0f);
		Rumbling::Once("ThighDropDown_L", &data.giant, shake_power, 0.10f, LNode, 0.0f);
	}

	void GTSBEH_Exit(AnimationEventData& data) {
		auto giant = &data.giant;
		ManageCamera(giant, false, CameraTracking::Thigh_Sandwich); // Un-Focus camera on AnimObjectA. Just to be Sure.
	}



	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////E V E N T S
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void ThighSandwichEnterEvent(const InputEventData& data) {
		auto& Sandwiching = ThighSandwichController::GetSingleton();
		auto pred = PlayerCharacter::GetSingleton();

		std::vector<Actor*> preys = Sandwiching.GetSandwichTargetsInFront(pred, 1);
		for (auto prey: preys) {
			Sandwiching.StartSandwiching(pred, prey);
			auto node = find_node(pred, "GiantessRune", false);
			if (node) {
				node->local.scale = 0.01f;
				update_node(node);
			}
		}
	}

	void ThighSandwichEnterEvent_Follower(const InputEventData& data) {
		Actor* pred = PlayerCharacter::GetSingleton();
		ForceFollowerAnimation(pred, FollowerAnimType::ThighSandwich);
	}


	void ThighSandwichAttackEvent(const InputEventData& data) {
		Actor* player = GetPlayerOrControlled();
		
		if (IsGtsBusy(player)) {
			float WasteStamina = 20.0f;
			if (Runtime::HasPerk(player, "KillerThighs")) {
				WasteStamina *= 0.65f;
			}
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				AnimationManager::StartAnim("ThighAttack", player);
			} else {
				if (IsThighSandwiching(player)) {
					NotifyWithSound(player, "You're too tired to perform thigh sandwich");
				}
			}
		}
	}

	void ThighSandwichHeavyAttackEvent(const InputEventData& data) {
		auto player = GetPlayerOrControlled();
		if (IsGtsBusy(player)) {
			float WasteStamina = 35.0f;
			if (Runtime::HasPerk(player, "KillerThighs")) {
				WasteStamina *= 0.65f;
			}
			if (GetAV(player, ActorValue::kStamina) > WasteStamina) {
				AnimationManager::StartAnim("ThighAttack_Heavy", player);
			} else {
				if (IsThighSandwiching(player)) {
					NotifyWithSound(player, "You're too tired to perform strong thigh sandwich");
				}
			}
		}
	}

	void ThighSandwichExitEvent(const InputEventData& data) {
		if (!IsFreeCameraEnabled()) {
			auto player = GetPlayerOrControlled();
			if (IsGtsBusy(player)) {
				AnimationManager::StartAnim("ThighExit", player);
			}
		}
	}
}

namespace GTS
{
	void AnimationThighSandwich::RegisterEvents() {
		InputManager::RegisterInputEvent("ThighSandwichEnter", ThighSandwichEnterEvent, ThighSandwitchCondition_Start);
		InputManager::RegisterInputEvent("PlayerThighSandwichEnter", ThighSandwichEnterEvent_Follower, ThighSandwitchCondition_Follower);
		InputManager::RegisterInputEvent("ThighSandwichAttack", ThighSandwichAttackEvent);
		InputManager::RegisterInputEvent("ThighSandwichAttackHeavy", ThighSandwichHeavyAttackEvent);
		InputManager::RegisterInputEvent("ThighSandwichExit", ThighSandwichExitEvent);

		AnimationManager::RegisterEvent("GTSSandwich_ThighImpact", "ThighSandwich", GTSSandwich_ThighImpact);
		AnimationManager::RegisterEvent("GTSSandwich_ThighImpact_H", "ThighSandwich", GTSSandwich_ThighImpact_H);
		AnimationManager::RegisterEvent("GTSSandwich_DropDown", "ThighSandwich", GTSSandwich_DropDown);
		AnimationManager::RegisterEvent("GTSSandwich_MoveLL_start", "ThighSandwich", GTSSandwich_MoveLL_start);
		AnimationManager::RegisterEvent("GTSSandwich_MoveLL_start_H", "ThighSandwich", GTSSandwich_MoveLL_start_H);
		AnimationManager::RegisterEvent("GTSSandwich_MoveLL_end", "ThighSandwich", GTSSandwich_MoveLL_end);
		AnimationManager::RegisterEvent("GTSSandwich_MoveLL_end_H", "ThighSandwich", GTSSandwich_MoveLL_end_H);
		AnimationManager::RegisterEvent("GTSSandwich_EnterAnim", "ThighSandwich", GTSSandwich_EnterAnim);
		AnimationManager::RegisterEvent("GTSSandwich_MoveBody_start", "ThighSandwich", GTSSandwich_MoveBody_start);
		AnimationManager::RegisterEvent("GTSSandwich_EnableRune", "ThighSandwich", GTSSandwich_EnableRune);
		AnimationManager::RegisterEvent("GTSSandwich_SitStart", "ThighSandwich", GTSSandwich_SitStart);
		AnimationManager::RegisterEvent("GTSSandwich_MoveBody_end", "ThighSandwich", GTSSandwich_MoveBody_end);
		AnimationManager::RegisterEvent("GTSSandwich_ThighLoop_Enter", "ThighSandwich", GTSSandwich_ThighLoop_Enter);
		AnimationManager::RegisterEvent("GTSSandwich_FootImpact", "ThighSandwich", GTSSandwich_FootImpact);
		AnimationManager::RegisterEvent("GTSSandwich_DisableRune", "ThighSandwich", GTSSandwich_DisableRune);
		AnimationManager::RegisterEvent("GTSSandwich_ThighLoop_Exit", "ThighSandwich", GTSSandwich_ThighLoop_Exit);
		AnimationManager::RegisterEvent("GTSSandwich_ExitAnim", "ThighSandwich", GTSSandwich_ExitAnim);
		AnimationManager::RegisterEvent("GTSBEH_Exit", "ThighSandwich", GTSBEH_Exit);
	}

	void AnimationThighSandwich::RegisterTriggers() {
		AnimationManager::RegisterTrigger("ThighEnter", "ThighSandwich", "GTSBEH_ThighSandwich_Start");
		AnimationManager::RegisterTrigger("ThighAttack", "ThighSandwich", "GTSBEH_ThighSandwich_Attack");
		AnimationManager::RegisterTrigger("ThighAttack_Heavy", "ThighSandwich", "GTSBEH_ThighSandwich_Attack_H");
		AnimationManager::RegisterTrigger("ThighExit", "ThighSandwich", "GTSBEH_ThighSandwich_ExitLoop");
	}

	AnimationThighSandwich& AnimationThighSandwich::GetSingleton() noexcept {
		static AnimationThighSandwich instance;
		return instance;
	}

	std::string AnimationThighSandwich::DebugName() {
		return "AnimationThighSandwich";
	}
}
