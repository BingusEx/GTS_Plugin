#include "Utils/QuestUtil.hpp"

namespace {

	const std::vector<std::string> AllPerks {
		"Acceleration",
		"AdditionalGrowth",
		"BendLifeless",
		"Breasts_Absorb",
		"Breasts_Intro",
		"Breasts_Mastery",
		"Breasts_Predominance",
		"Breasts_Suffocate",
		"Breasts_Vore",
		"ButtCrush_GrowingDisaster",
		"ButtCrush_KillerBooty",
		"ButtCrush_LoomingDoom",
		"ButtCrush_NoEscape",
		"ButtCrush_UnstableGrowth",
		"CalamityPerk",
		"ColossalGrowth",
		"Cruelty",
		"DarkArts",
		"DarkArts_Aug",
		"DarkArts_Aug2",
		"DarkArts_Aug3",
		"DarkArts_Max",
		"DestructionBasics",
		"DevastatingSprint",
		"DisastrousTremor",
		"EnhancedCapacity",
		"EternalCalamity",
		"ExtraGrowth",
		"ExtraGrowthMax",
		"FastShrink",
		"Gluttony",
		"GrowingPressure",
		"GrowthDesirePerk",
		"GrowthDesirePerkAug",
		"GrowthOfStrength",
		"GrowthOnHitPerk",
		"HealthGate",
		"HealthRegenPerk",
		"hhBonus",
		"HugCrush",
		"HugCrush_Greed",
		"HugCrush_HugsOfDeath",
		"HugCrush_LovingEmbrace",
		"HugCrush_MightyCuddles",
		"HugCrush_ToughGrip",
		"KillerThighs",
		"LaunchDamage",
		"LethalShrink",
		"LifeAbsorption",
		"LifeSteal",
		"MightOfDragons",
		"MightOfGiants",
		"MightyLegs",
		"NoSpeedLoss",
		"OnTheEdge",
		"QuickApproach",
		"RandomGrowth",
		"RandomGrowthAug",
		"RandomGrowthTerror",
		"RavagingInjuries",
		"RealCruelty",
		"RumblingFeet",
		"ShrinkingGaze",
		"SizeConversion",
		"SizeReserve",
		"SizeReserveAug",
		"SizeReserveAug2",
		"SkilledGTS",
		"SoulVorePerk",
		"StaggerImmunity",
		"TalkToActor",
		"TrueGiantess",
		"VorePerk",
		"WrathfulCalamity",
		"SizeManipulation",
		"ExpandedGrowth",
		"Breasts_Strangle"
	};

	const std::vector<std::string> AllSpells {

	};
}


namespace GTS {

	void SkipProgressionQuest() {

		auto progressionQuest = Runtime::GetQuest("MainQuest");
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_SkipQuest");
		}
	}

	bool ProgressionQuestCompleted() {
		auto Quest = Runtime::GetQuest("MainQuest");

		if (Quest) {
			return Quest->GetCurrentStageID() >= 200;
		}

		return false;

	}

	void GiveAllPerksToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (auto& Perk : AllPerks) {
			Runtime::AddPerk(Player, Perk);
		}

		Notify("All perks have been given.");
	}

	void GiveAllSpellsToPlayer() {
		auto Player = PlayerCharacter::GetSingleton();

		for (auto& Perk : AllSpells) {
			Runtime::AddSpell(Player, Perk);
		}

		Notify("All spells have been given.");
	}

	void GiveAllShoutsToPlayer() {
		auto progressionQuest = Runtime::GetQuest("MainQuest");
		if (progressionQuest) {
			CallFunctionOn(progressionQuest, "GTSProgressionQuest", "Proxy_GetAllShouts");
			Notify("All shouts have been given.");
		}

	}
}