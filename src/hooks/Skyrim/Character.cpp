#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/SizeHitEffects.hpp"
#include "Hooks/Skyrim/Character.hpp"
#include "managers/Attributes.hpp"
#include "data/runtime.hpp"
#include "data/persistent.hpp"
#include "data/plugin.hpp"

#include "scale/scale.hpp"



using namespace GTS;

namespace {

	float Get_Speed_Override(Actor* giant, float incoming_speed) {
		float new_speed = incoming_speed;
		if (giant->formID != 0x14 && IsTeammate(giant) && IsHuman(giant) && IsFemale(giant, true) && get_visual_scale(giant) > 1.5f) {
			float speed_cap = 400.0f / get_visual_scale(giant);
			float speed = AnimationManager::GetAnimSpeed(giant);
			new_speed *= speed * speed;
			//log::info("Adjusting speed for {}", giant->GetDisplayFullName());
			if (giant->AsActorState()->IsSprinting()) {
				speed_cap *= 2.0f;
			}
			if (new_speed < speed_cap) {
				return speed_cap;
			}
		}
		return new_speed;
	}

}

namespace Hooks {

	void Hook_Character::Hook() {
		log::info("Hooking Character");
		REL::Relocation<std::uintptr_t> Vtbl{ RE::VTABLE_Character[0] };
		// _GetBoundMin = Vtbl.write_vfunc(0x73, GetBoundMin);
		// _GetBoundMax = Vtbl.write_vfunc(0x74, GetBoundMax);
		_HandleHealthDamage = Vtbl.write_vfunc(REL::Relocate(0x104, 0x104, 0x106), HandleHealthDamage);
		_AddPerk = Vtbl.write_vfunc(REL::Relocate(0x0FB, 0x0FB, 0x0FD), AddPerk);
		_RemovePerk = Vtbl.write_vfunc(REL::Relocate(0x0FC, 0x0FC, 0x0FE), RemovePerk);
		_Move = Vtbl.write_vfunc(REL::Relocate(0x0C8, 0x0C8, 0x0CA), Move);
		//_ProcessTracking = (Vtbl.write_vfunc(REL::Relocate(0x122, 0x122, 0x124), ProcessTracking));

		REL::Relocation<std::uintptr_t> Vtbl5{ RE::VTABLE_Character[5] };
		_GetActorValue = Vtbl5.write_vfunc(0x01, GetActorValue);
		_GetPermanentActorValue = Vtbl5.write_vfunc(0x02, GetPermanentActorValue);
		_GetBaseActorValue = Vtbl5.write_vfunc(0x03, GetBaseActorValue);
		_SetBaseActorValue = Vtbl5.write_vfunc(0x04, SetBaseActorValue);

		REL::Relocation<std::uintptr_t> AnimVtbl{ RE::VTABLE_Character[2] };
		_NPCAnimEvents = AnimVtbl.write_vfunc(0x1, &NPCAnimEvents);
	}

	void Hook_Character::HandleHealthDamage(Character* a_this, Character* a_attacker, float a_damage) {
		if (a_attacker) {
			SizeHitEffects::GetSingleton().ApplyEverything(a_attacker, a_this, a_damage); // Apply bonus damage, overkill, stagger resistance
			if (Runtime::HasPerkTeam(a_this, "SizeReserveAug")) { // Size Reserve Augmentation
				auto Cache = Persistent::GetSingleton().GetData(a_this);
				if (Cache) {
					Cache->SizeReserve += -a_damage/3000;
				}
			}
		}
		_HandleHealthDamage(a_this, a_attacker, a_damage);  // Just reports the value, can't override it.
	}

	void Hook_Character::AddPerk(Character* a_this, BGSPerk* a_perk, std::uint32_t a_rank) {
		_AddPerk(a_this, a_perk, a_rank);
		AddPerkEvent evt = AddPerkEvent {
			.actor = a_this,
			.perk = a_perk,
			.rank = a_rank,
		};
		EventDispatcher::DoAddPerk(evt);
	}

	void Hook_Character::RemovePerk(Character* a_this, BGSPerk* a_perk) {
		RemovePerkEvent evt = RemovePerkEvent {
			.actor = a_this,
			.perk = a_perk,
		};
		EventDispatcher::DoRemovePerk(evt);
		_RemovePerk(a_this, a_perk);
	}

	float Hook_Character::GetActorValue(ActorValueOwner* a_owner, ActorValue a_akValue) { // Override Carry Weight and Damage
		float value = _GetActorValue(a_owner, a_akValue);
		if (Plugin::InGame()) {
			Actor* a_this = skyrim_cast<Actor*>(a_owner);
			if (a_this) {
				if (a_akValue == ActorValue::kCarryWeight) {
					value = AttributeManager::AlterGetAv(a_this, a_akValue, value);
				}
				if (a_akValue == ActorValue::kSpeedMult) {
					value = Get_Speed_Override(a_this, value);
				}
			}
		}
		return value;
	}

	float Hook_Character::GetBaseActorValue(ActorValueOwner* a_owner, ActorValue a_akValue) { // Override Health
		float value = _GetBaseActorValue(a_owner, a_akValue);
		if (Plugin::InGame()) {
			Actor* a_this = skyrim_cast<Actor*>(a_owner);
			float bonus = 1.0f;
			if (a_this) {
				if (a_akValue == ActorValue::kCarryWeight) {
					value = AttributeManager::AlterGetBaseAv(a_this, a_akValue, value);
				}
			}
		}
		return value;
	}

	void Hook_Character::SetBaseActorValue(ActorValueOwner* a_owner, ActorValue a_akValue, float value) {
		if (Plugin::InGame()) {
			Actor* a_this = skyrim_cast<Actor*>(a_owner);
			if (a_this) {
				if (a_akValue == ActorValue::kCarryWeight) {
					value = AttributeManager::AlterSetBaseAv(a_this, a_akValue, value);
				}
			}
		}
		_SetBaseActorValue(a_owner, a_akValue, value);
	}

	float Hook_Character::GetPermanentActorValue(ActorValueOwner* a_owner, ActorValue a_akValue) { // Override Carry Weight and Damage
		float value = _GetPermanentActorValue(a_owner, a_akValue);
		if (Plugin::InGame()) {
			Actor* a_this = skyrim_cast<Actor*>(a_owner);
			if (a_this) {
				if (a_akValue == ActorValue::kCarryWeight) {
					value = AttributeManager::AlterGetPermenantAv(a_this, a_akValue, value);
				}
			}
		}
		return value;
	}

	void Hook_Character::Move(Character* a_this, float a_arg2, const NiPoint3& a_position) { // Override Movement Speed
		float alter = 1.0f;
		if (Plugin::InGame()) {
			if (a_this->IsInKillMove()) {
				return _Move(a_this, a_arg2, a_position); // Do nothing in Kill moves
			}
			if (a_this && a_this->Get3D1(false)) {
				if (a_this->Is3DLoaded()) {
					alter = AttributeManager::AlterMovementSpeed(a_this, a_position);
				}
			}
		}
		return _Move(a_this, a_arg2, a_position * alter);
	}

	void Hook_Character::NPCAnimEvents(BSTEventSink<BSAnimationGraphEvent>* a_this, BSAnimationGraphEvent& a_event, BSTEventSource<BSAnimationGraphEvent>* a_src) {
		if (a_event.tag != NULL && a_event.holder != NULL) {
			Actor* actor = static_cast<Actor*>(a_this);
			if (actor) {
				EventDispatcher::DoActorAnimEvent(actor, a_event.tag, a_event.payload);
			}
		}
		return _NPCAnimEvents(a_this, a_event, a_src);
	}

	NiPoint3 Hook_Character::GetBoundMax(Character* a_this) {
		auto bound = _GetBoundMax(a_this);
		if (a_this) {
			float scale = get_giantess_scale(a_this);
			if (scale > 1e-4) {
				bound = bound * scale;
			}
		}
		return bound;
	}

	NiPoint3 Hook_Character::GetBoundMin(Character* a_this) {
		auto bound = _GetBoundMin(a_this);
		if (a_this) {
			float scale = get_giantess_scale(a_this);
			if (scale > 1e-4) {
				bound = bound * scale;
			}
		}
		return bound;
	}
}
