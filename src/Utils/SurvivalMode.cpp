#include "Utils/SurvivalMode.hpp"

namespace GTS {

	void SurvivalMode_RemoveAllSpells(Actor* actor, SpellItem* stage0, SpellItem* stage1, SpellItem* stage2, SpellItem* stage3, SpellItem* stage4, SpellItem* stage5) {
		if (Runtime::IsSurvivalModeInstalled()) {

			actor->RemoveSpell(stage0);
			actor->RemoveSpell(stage1);
			actor->RemoveSpell(stage2);
			actor->RemoveSpell(stage3);
			actor->RemoveSpell(stage4);
			actor->RemoveSpell(stage5);
		}
	}

	void SurvivalMode_RefreshSpells(Actor* actor, float currentvalue) {

		if (Runtime::IsSurvivalModeInstalled()) {

			auto stage0 = Runtime::GetSpell("Survival_HungerStage0");
			auto stage1 = Runtime::GetSpell("Survival_HungerStage1");
			auto stage2 = Runtime::GetSpell("Survival_HungerStage2");
			auto stage3 = Runtime::GetSpell("Survival_HungerStage3");
			auto stage4 = Runtime::GetSpell("Survival_HungerStage4");
			auto stage5 = Runtime::GetSpell("Survival_HungerStage5");

			auto stage1threshold = Runtime::GetFloat("Survival_HungerStage1Value");
			auto stage2threshold = Runtime::GetFloat("Survival_HungerStage2Value");
			auto stage3threshold = Runtime::GetFloat("Survival_HungerStage3Value");
			auto stage4threshold = Runtime::GetFloat("Survival_HungerStage4Value");
			auto stage5threshold = Runtime::GetFloat("Survival_HungerStage5Value");

			SurvivalMode_RemoveAllSpells(actor, stage0, stage1, stage2, stage3, stage4, stage5);

			if (currentvalue <= stage1threshold) {
				Runtime::AddSpell(actor, "Survival_HungerStage1");
			}
			else if (currentvalue <= stage2threshold) {
				Runtime::AddSpell(actor, "Survival_HungerStage2");
			}
			else if (currentvalue <= stage3threshold) {
				Runtime::AddSpell(actor, "Survival_HungerStage3");
			}
			else if (currentvalue <= stage4threshold) {
				Runtime::AddSpell(actor, "Survival_HungerStage4");
			}
			else if (currentvalue <= stage5threshold) {
				Runtime::AddSpell(actor, "Survival_HungerStage5");
			}
		}
	}

	void SurvivalMode_AdjustHunger(Actor* giant, float tinyscale, bool IsLiving, bool Finished) {

		if (giant->formID == 0x14) {

			if (Runtime::IsSurvivalModeInstalled()) {

				if (!AllowDevourment()) { // Seems like Devourment already manages Hunger system, no need to touch it in that case
					auto Survival = Runtime::GetGlobal("Survival_ModeEnabled");
					if (!Survival) {
						return; // Abort if it doesn't exist (Should fix issues if we don't have AE CC mods)
					}
					float SurvivalEnabled = Runtime::GetBool("Survival_ModeEnabled");
					if (SurvivalEnabled) {
						auto HungerNeed = Runtime::GetGlobal("Survival_HungerNeedValue"); // Obtain
						float restore = 128.0f;

						if (!IsLiving) {
							tinyscale *= 0.20f; // Less effective on non living targets
						}
						if (Finished) {
							restore *= 6.0f; // Stronger gain on Vore Finish
						}

						float power = (get_visual_scale(giant) / tinyscale); // Get size difference and * it by natural size

						HungerNeed->value -= (restore / power);
						if (HungerNeed->value <= 0.0f) {
							HungerNeed->value = 0.0f; // Cap it at 0.0
						}
						float value = HungerNeed->value;
						SurvivalMode_RefreshSpells(giant, value);
					}
				}
			}
		}
	}
}