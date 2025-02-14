#include "Hooks/Skyrim/PreventAnimations.hpp"
#include "Hooks/Hooks.hpp"

#include "Data/Transient.hpp"

#include "Scale/Scale.hpp"

using namespace RE;
using namespace GTS;

namespace {
	
    const float KillMove_Threshold_High = 2.00f; // If GTS/Tiny size ratio is > than 2 times = disallow killmove on Tiny
    const float KillMove_Threshold_Low = 0.75f; // If Tiny/GTS size ratio is < than 0.75 = disallow killmove on GTS

	// Actions that we want to prevent
	const auto DefaultSheathe = 			0x46BB2;
	const auto JumpRoot =					0x88302;
	const auto NonMountedDraw = 			0x1000992;
	const auto NonMountedForceEquip = 		0x1000993;
	const auto JumpStandingStart =        	0x884A2;   // 558242
	const auto JumpDirectionalStart =       0x884A3;   // 558243

	const auto JumpFall =                   0xA791D;   // 686365 
	const auto FallRoot =                   0xA790E;   // 686350 // The "falling down" anim

	// Killmoves that we want to prevent
	const auto KillMoveFrontSideRoot =      0x24CD4;
	const auto KillMoveDragonToNPC =        0xC1F20;
	const auto KillMoveRootDragonFlight =   0xC9A1B;
	const auto KillMoveBackSideRoot =       0xE8458;
	const auto KillMoveFrontSideRoot00 =    0x100e8B;
	const auto KillMoveBackSideRoot00 =     0x100F16;

	// Attacks that we want to prevent

	const auto MountedDraw = 16779659;
	const auto DrawMagic = 561168;
	const auto DefaultDrawWeapon = 561169;
	//const auto NonMountedDraw = 16779666;
	const auto DrawSheathRoot = 78512;

	// Magic
	const auto CastDualMagic = 220049;
	const auto AttackMagicLeftRoot = 116570;
	const auto AttackMagicRightRoot = 116321;


	// Left Hand
	const auto BowLeftAttack = 619835;
	const auto LeftHandAttack = 765123;
	const auto AttackLeftRoot = 78358;
	const auto LeftAttackRelease = 16779651;
	const auto ReleaseLeftRoot = 78940;
	// Normal attack
	const auto NormalAttack = 78357;

	// Right Hand
	const auto AttackRightRoot = 78353;
	const auto RightAttackRelease = 83292;
	const auto ReleaseRightRoot = 78943;
	// Power attacks
	const auto PowerAttack = 951382;
	const auto PowerBash = 951378;
	const auto PowerAttackRoot = 78724;
	// Sprint
	const auto SprintStart = 1069299;
	const auto SprintRootStart =            0x03B4A8;
	const auto SprintRootStop = 78362;
	// Turn

	const auto NPCTurnToWalk =  	0x02EDE6;
	const auto NPCTurnInPlace = 	0x02EDBC;
	const auto NPCPathStartRoot = 	0x02EDBA;

	// Blocking
	const auto BlockHit = 80631;
	const auto BlockHitRoot = 80630;

	bool ShouldBlockFalling(Actor* actor) {
		bool block = false;
		auto charCont = actor->GetCharController();
		if (charCont) {
			float scale = get_visual_scale(actor);
			float falltime = charCont->fallTime;
			float threshold = 0.04f * scale;
			//log::info("Fall time of {} is {}", actor->GetDisplayFullName(), falltime);
			if (falltime < threshold) {
				//log::info("Blocking anim");
				block = true;
			}
		}
		return block;
	}

	bool PreventJumpFall(FormID idle, Actor* performer) {
		if (idle == FallRoot) {
			//log::info("Checking fall root");
			return ShouldBlockFalling(performer);
		}
		return false;
	}

	bool PreventKillMove(FormID idle, ConditionCheckParams* params, Actor* performer, TESObjectREFR* victim) {
		// KillMoves
		
		bool KillMove = false;
		bool Block = false;

		switch (idle) {
			case KillMoveFrontSideRoot:
				KillMove = true;
			break;	
			case KillMoveDragonToNPC:
				KillMove = true;
			break;
			case KillMoveRootDragonFlight:
				KillMove = true;
			break;
			case KillMoveBackSideRoot:
				KillMove = true;
			break;
			case KillMoveFrontSideRoot00:
				KillMove = true;
			break;
			case KillMoveBackSideRoot00:
				KillMove = true;
			break;
		}

		if (KillMove) {
			if (victim) {
				Actor* victimref = skyrim_cast<Actor*>(victim);
				if (victimref) {
					float size_difference = GetSizeDifference(performer, victimref, SizeType::GiantessScale, true, false);
					if (size_difference > KillMove_Threshold_High || size_difference < KillMove_Threshold_Low) {
						Block = true;
					}
				}
			}
		}

		return Block;
	}

	bool PreventSprinting(FormID idle, Actor* performer) {
		if (idle == SprintRootStart) {
			if (IsTeammate(performer) && get_visual_scale(performer) > 2.0f) {
				return true;
			}
			return false;
		}
		return false;
	}

	bool IsDisallowed(FormID idle) {
		switch (idle) {
			case DefaultSheathe:
				//log::info("Block DefaultSheathe");
				return true;
			break;	
			case JumpRoot:
				//log::info("Block JumpRoot");
				return true;
			break;	
			case NonMountedDraw:
				//log::info("Block NonMountedDraw");
				return true;
			break;	
			case NonMountedForceEquip:
				//log::info("Block NonMountedForceEquip");
				return true;
			break;	
			case JumpStandingStart:
				//log::info("Block JumpStandingStart");
				return true;	
			break;	
			case JumpDirectionalStart:
				//log::info("Block JumpDirectionalStart");
				return true;	
			break;
			return false;
		}
		return false;
	}

	bool BlockAnimation(TESIdleForm* idle, ConditionCheckParams* params) {
		if (!idle) {
			return false;
		}

		auto Form = idle->formID;

		Actor* performer = params->actionRef->As<RE::Actor>();

		if (performer) {
			if (PreventKillMove(Form, params, performer, params->targetRef)) {
				//log::info("KILLMOVE PREVENTED");
				//log::info("Block PreventKillMove");
				return true;
			}

			if (IsThighSandwiching(performer)) { // Disallow anims in these 2 cases 
				//log::info("Block IsThighSandwiching");
				return IsDisallowed(Form);
			} if (IsBetweenBreasts(performer)) {
				//log::info("Block IsBetweenBreasts");
				return IsDisallowed(Form);
			}
			
			if (PreventJumpFall(Form, performer)) {
				//log::info("Block PreventJumpFall");
				return true; // Disable fall down anim for GTS so it won't look off/annoying at large scales
			}

			if (performer->formID != 0x14 && PreventSprinting(Form, performer)) {
				return true;
			}

			if (performer->formID == 0x14 && IsGtsBusy(performer) && IsFreeCameraEnabled()) {
				//log::info("Block performer->formID == 0x14 && IsGtsBusy(performer) && IsFreeCameraEnabled()");
				return IsDisallowed(Form); 							// One of cases when we alter anims for Player. 
				// Needed because it's problematic to disallow specific controls through controls.hpp
			}

			if (!IsGtsBusy(performer) && !IsProning(performer)) {
                // Do not affect non-gts-busy actors!
				return false;
			}

			return IsDisallowed(Form);
			//log::info("Blocking anims for {}", performer->GetDisplayFullName());
		}
		return false;
	}
}

namespace Hooks {

	using namespace GTS;
	using namespace RE;

	void Hook_PreventAnimations::Hook(Trampoline& trampoline) { 
        static FunctionHook<TESIdleForm*(TESIdleForm* a_this, ConditionCheckParams* params, void* unk3)>IdleFormHook (        
			REL::RelocationID(24067, 24570),
			// 24067 = sub_140358150 (SE)
			// 24570 = FUN_14036ec80 (AE)
			[](TESIdleForm* a_this, ConditionCheckParams* params, void* unk3) {
                // Return nullptr When we don't want specific anims to happen
                // This hook prevents them from playing (KillMoves and Sheathe/Unsheathe/Jump/Jump Root anims)
				
				auto* result = IdleFormHook(a_this, params, unk3);

				if (a_this) {
					if (BlockAnimation(a_this, params)) {
						/*auto* EventName = a_this->GetFormEditorID();
						Actor* performer = params->actionRef->As<RE::Actor>();
						if (performer) {
							log::info("Blocking anim: {} of {}", EventName, performer->GetDisplayFullName());
						}*/
						result = nullptr; // cancel anim
					}
				}

				return result;  
            }
        );
    }
}