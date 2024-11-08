#include "managers/RipClothManager.hpp"

#include "managers/GtsSizeManager.hpp"
#include "ActionSettings.hpp"
#include "managers/GtsManager.hpp"
#include "magic/effects/common.hpp"
#include "scale/scale.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "data/time.hpp"
#include "timer.hpp"
#include "timer.hpp"
#include "managers/Rumble.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"

using namespace RE;
using namespace Gts;

namespace Gts {

    #define RANDOM_OFFSET (RandomFloat(0.01, 0.09))
	static bool RipClothManagerActive;

	// List of keywords (Editor ID's) we want to ignore when stripping
	static const std::vector<string> KeywordBlackList = {
		"SOS_Genitals", //Fix Slot 52 Genitals while still keeping the ability to unequip slot 52 underwear
		"ArmorJewelry",
		"VendorItemJewelry"
		"ClothingRing",
		"ClothingNecklace",
		"SexLabNoStrip", //This is the keyword 3BA uses for the SMP addons?, it doesnt even originate from SL.
		"GTSDontStrip"
	};

	//List of slots we want to check
	static const std::vector<BGSBipedObjectForm::BipedObjectSlot> VallidSlots = {
		BGSBipedObjectForm::BipedObjectSlot::kHead,                 // 30
		// BGSBipedObjectForm::BipedObjectSlot::kHair,					// 31
		BGSBipedObjectForm::BipedObjectSlot::kBody,                 // 32
		BGSBipedObjectForm::BipedObjectSlot::kHands,                // 33
		BGSBipedObjectForm::BipedObjectSlot::kForearms,             // 34
		// BGSBipedObjectForm::BipedObjectSlot::kAmulet,					// 35
		// BGSBipedObjectForm::BipedObjectSlot::kRing,					// 36
		BGSBipedObjectForm::BipedObjectSlot::kFeet,                 // 37
		BGSBipedObjectForm::BipedObjectSlot::kCalves,               // 38
		// BGSBipedObjectForm::BipedObjectSlot::kShield,					// 39
		// BGSBipedObjectForm::BipedObjectSlot::kTail,					// 40
		// BGSBipedObjectForm::BipedObjectSlot::kLongHair,				// 41
		BGSBipedObjectForm::BipedObjectSlot::kCirclet,              // 42
		BGSBipedObjectForm::BipedObjectSlot::kEars,                 // 43
		BGSBipedObjectForm::BipedObjectSlot::kModMouth,             // 44
		BGSBipedObjectForm::BipedObjectSlot::kModNeck,              // 45
		BGSBipedObjectForm::BipedObjectSlot::kModChestPrimary,      // 46
		BGSBipedObjectForm::BipedObjectSlot::kModBack,              // 47
		BGSBipedObjectForm::BipedObjectSlot::kModMisc1,             // 48
		BGSBipedObjectForm::BipedObjectSlot::kModPelvisPrimary,     // 49
		// BGSBipedObjectForm::BipedObjectSlot::kDecapitateHead,			// 50
		// BGSBipedObjectForm::BipedObjectSlot::kDecapitate,				// 51
		BGSBipedObjectForm::BipedObjectSlot::kModPelvisSecondary,   // 52
		BGSBipedObjectForm::BipedObjectSlot::kModLegRight,			// 53
		BGSBipedObjectForm::BipedObjectSlot::kModLegLeft,           // 54
		BGSBipedObjectForm::BipedObjectSlot::kModFaceJewelry,		// 55
		BGSBipedObjectForm::BipedObjectSlot::kModChestSecondary,    // 56
		BGSBipedObjectForm::BipedObjectSlot::kModShoulder,          // 57
		BGSBipedObjectForm::BipedObjectSlot::kModArmLeft,			// 58
		BGSBipedObjectForm::BipedObjectSlot::kModArmRight,          // 59
		// BGSBipedObjectForm::BipedObjectSlot::kModMisc2,				// 60
		// BGSBipedObjectForm::BipedObjectSlot::kFX01,					// 61
	};

	ClothManager& ClothManager::GetSingleton() noexcept {
		static ClothManager instance;
		return instance;
	}

	std::string ClothManager::DebugName() {
		return "ClothManager";
	}
	//Add A check bool to transient to decect npc_reequips

	//I don't like this. Ideally we should call the same update func that the game does when the npc changes cells for example.
	//But alas i have no idea how to do that :(
	//This will equip all armor in the inventory
	void ReEquipClothing(Actor* a_actor) {
		//log::info("ReEquip: {}", a_actor->GetName());
		const auto inv = a_actor->GetInventory();
		for (const auto& [item, invData] : inv) {
			const auto& [count, entry] = invData;
			if (count > 0) {
				if (item->As<RE::TESObjectARMO>()) {
					for (const auto& xList : *entry->extraLists) {
						RE::ActorEquipManager::GetSingleton()->EquipObject(a_actor, item, xList, 1, nullptr, false, false, false);
					}
				}
			}
		}
	}



	//Have We Shrinked Since The Last Update?
	bool IsShrinking(Actor* a_actor, float Scale) {

		if (!a_actor) return false;

		auto transient = Transient::GetSingleton().GetActorData(a_actor);
		if (!transient) return false;
		//Add this to transient
		// If Current Scale is Equal or Larger, we either growed or stayed the same so no shriking happened
		bool Shrinking = !(Scale >= transient->rip_lastScale);
		transient->rip_lastScale = Scale;
		//log::info("Shrinking: {}", Shrinking);
		return Shrinking;
	}

	void RipRandomClothing(RE::Actor* a_actor) {

		if(!a_actor) {
			return;
		}

		std::vector<TESObjectARMO*> ArmorList;
		for (auto Slot : VallidSlots) {

			auto Armor = a_actor->GetWornArmor(Slot);
			// If armor is null skip
			if (!Armor) {
				continue;
			}

			for (const auto& BKwd : KeywordBlackList) {
				if (Armor->HasKeywordString(BKwd)) {
					goto BKwd_Skip; //If blacklisted keyword is found skip
				}
				                                  
			}

			//Else Add it to the vector
			ArmorList.push_back(Armor);

			//Funny label
		BKwd_Skip:
			continue;
		}

		int ArmorCount = ArmorList.size();
		if (ArmorCount == 0) {
			return; //No Vallid Armors Found
		}

		// Select Random Armor To Possibly Remove
		int idx = RandomInt(0, ArmorCount - 1);

		TESObjectARMO* tesarmo = ArmorList[idx];
		if (!tesarmo) return;


		auto manager = RE::ActorEquipManager::GetSingleton();
		manager->UnequipObject(a_actor, tesarmo, nullptr, 1, nullptr, true, false, false);

		PlayMoanSound(a_actor, 0.7);
		Task_FacialEmotionTask_Moan(a_actor, 1.0, "RipCloth");
		Runtime::PlaySound("ClothTearSound", a_actor, 0.7, 1.0);
		Rumbling::Once("ClothManager", a_actor, Rumble_Misc_TearClothes, 0.075);
	}

	void RipAllClothing(RE::Actor* a_actor) {

		if (!a_actor) {
			return;
		}

		auto manager = RE::ActorEquipManager::GetSingleton();
		bool Ripped = false;

		for (auto Slot : VallidSlots) {

			TESObjectARMO* Armor = a_actor->GetWornArmor(Slot);
			// If armor is null skip
			if (!Armor) {
				continue;
			}

			for (const auto& BKwd : KeywordBlackList) {
				if (Armor->HasKeywordString(BKwd)) {
					//Simplest way to break the parent loop
					goto BKwd_Skip;
				}
			}
			
			manager->UnequipObject(a_actor, Armor, nullptr, 1, nullptr, true, false, false);
			Ripped = true;

			//Funny label
			BKwd_Skip:
			     continue;
		}

		if (Ripped) {
			PlayMoanSound(a_actor, 1.0);
			Task_FacialEmotionTask_Moan(a_actor, 1.0, "RipCloth");
			Runtime::PlaySound("ClothTearSound", a_actor, 1.0, 1.0);
			Rumbling::Once("ClothManager", a_actor, Rumble_Misc_TearAllClothes, 0.095);
		}
	}

	void ClothManager::CheckClothingRip(Actor* a_actor) {

		if (!a_actor) return;

		RipClothManagerActive = (Runtime::GetFloat("AllowClothTearing") > 0.0f);
		if (!RipClothManagerActive || (!IsTeammate(a_actor) && a_actor->formID != 0x14)) return;

		static Timer timer = Timer(1.2);
		if (!timer.ShouldRunFrame()) return;

		auto actordata = Persistent::GetSingleton().GetActorData(a_actor);
		if (!actordata) return;

		float CurrentScale = get_visual_scale(a_actor);

		if (CurrentScale < rip_threshold) {
			//If Smaller than rip_threshold but offset was > 0 means we shrunk back down, so reset the offset
			if (actordata->rip_offset > 0.0f) {
				actordata->rip_offset = 0.0f;

				//ReEquip Ripped Clothing On Follower NPC's
				if (a_actor->formID != 0x14 && IsTeammate(a_actor)) {
					ReEquipClothing(a_actor);
				}
			}
			return;
		}

		//Rip Immediatly if too big.
		//Its a bit wastefull but allows us to imediatly unequip if the player equips something again
		if (CurrentScale > rip_toobig) {
			RipAllClothing(a_actor);
			return;
		}

		//Actor is shrinking don't rip
		if (IsShrinking(a_actor,CurrentScale)) {
			actordata->rip_offset = CurrentScale - rip_threshold + RANDOM_OFFSET;
			//log::info("OffsetAfterShrink {}", actordata->rip_offset);
			return;
		}

		

		float Offs = RANDOM_OFFSET;
		//if we meet scale conditions
		if (CurrentScale >= (rip_threshold + actordata->rip_offset + Offs)) {
			actordata->rip_offset = CurrentScale - rip_threshold + Offs;
			//log::info("Offset After Rip {}", actordata->rip_offset);
			RipRandomClothing(a_actor);
			return;
		}
	}

	bool ClothManager::ShouldPreventReEquip(Actor* a_actor, RE::TESBoundObject* a_object) {
		//If anthing is invallid let the native code handle it.
		if (!a_actor || !a_object) return false;

		//Cached value instead of getting the variable directly. 
		//The Check can get spammed by the Equip hook when a lot of actors are around.
		//If clothing rip is disabled or is not a follower always allow
		if (!RipClothManagerActive || (!IsTeammate(a_actor) && a_actor->formID != 0x14)) return false;

		//if smaller than rip_threhsold or target actor is the player allow re-equip
		if (get_visual_scale(a_actor) < rip_threshold || a_actor->formID == 0x14) {
			return false;
		}

		auto tesarmo = static_cast<TESObjectARMO*>(a_object);

		//if the item is not an armor, allow it
		if (!tesarmo) return false;
		for (auto Slot : VallidSlots) {
			//For each vallid slot check if the to be equiped cotains said slot
			if (tesarmo->bipedModelData.bipedObjectSlots.any(Slot)) {

				//if it does check the keywords as a last resort
				for (const auto& BKwd : KeywordBlackList) {
					if (tesarmo->HasKeywordString(BKwd)) {
						return false; //If blacklisted keyword is found do not strip
					}
				}
				return true;
			}
		}
		//if not within a vallid slot, allow.
		return false;
	}
}
