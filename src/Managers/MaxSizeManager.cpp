#include "Managers/MaxSizeManager.hpp"
#include "Config/Config.hpp"
#include "Managers/AI/aifunctions.hpp"

using namespace GTS;

namespace {

    constexpr float DEFAULT_MAX = 1000000.0f;

    float get_endless_height(Actor* actor) {
		float endless = 0.0f;

		if (Runtime::HasPerk(actor, "ColossalGrowth")) {
			endless = DEFAULT_MAX;
		}

		return endless;
	}

    float get_default_size_limit(float NaturalScale, float BaseLimit) { // default size limit for everyone
        float size_calc = NaturalScale + ((BaseLimit - 1.0f) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f, DEFAULT_MAX);

        return GetLimit;
    }

    float get_mass_based_limit(Actor* actor, float NaturalScale) { // get mass based size limit for Player if using Mass Based mode
        float low_limit = get_endless_height(actor);

        if (low_limit <= 0.0f) {

            low_limit = Persistent::GetSingleton().GTSGlobalSizeLimit.value; // Cap max size through normal size rules
            // Else max possible size is unlimited
        }

        float size_calc = NaturalScale + Persistent::GetSingleton().GTSMassBasedSize.value * NaturalScale;
        float GetLimit = std::clamp(size_calc, NaturalScale, low_limit);

        return GetLimit;
    }

    float get_follower_size_limit(float NaturalScale, float FollowerLimit) { // Self explanatory
        float size_calc = NaturalScale + ((FollowerLimit) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f * FollowerLimit, DEFAULT_MAX);

        return GetLimit;
    }

    float get_npc_size_limit(float NaturalScale, float NPCLimit) { // get non-follower size limit
        float size_calc = NaturalScale + ((NPCLimit - 1.0f) * NaturalScale);
		float GetLimit = std::clamp(size_calc, 1.0f * NPCLimit, DEFAULT_MAX);

        return GetLimit;
    }
}

namespace GTS {

    void UpdateMaxScale() {

        auto profiler = Profilers::Profile("SizeManager: Update");

		for (auto actor: find_actors()) {

			float Endless = 0.0f;

			if (actor->formID == 0x14) {
				Endless = get_endless_height(actor);
			}

            float NaturalScale = get_natural_scale(actor, true);
            float QuestStage = Runtime::GetStage("MainQuest");

            float BaseLimit = Persistent::GetSingleton().GTSGlobalSizeLimit.value;

            float NPCLimit = Config::GetBalance().fMaxOtherSize;

			const bool IsMassBased = Config::GetBalance().sSizeMode == "kMassBased"; // Should DLL use mass based formula for Player?

            float FollowerLimit = Config::GetBalance().fMaxFollowerSize;

            float GetLimit = get_default_size_limit(NaturalScale, BaseLimit); // Default size limit
			
			if (actor->formID == 0x14 && IsMassBased) { 
				GetLimit = get_mass_based_limit(actor, NaturalScale); // Apply Player Mass-Based max size
			}
			else if (QuestStage > 100 && FollowerLimit > 0.0f && FollowerLimit != 1.0f && actor->formID != 0x14 && IsTeammate(actor)) { 
				GetLimit = get_follower_size_limit(NaturalScale, FollowerLimit); // Apply Follower Max Size
			}
			else if (QuestStage > 100 && NPCLimit > 0.0f && NPCLimit != 1.0f && actor->formID != 0x14 && !IsTeammate(actor)) { 
                GetLimit = get_npc_size_limit(NaturalScale, NPCLimit); // Apply Other NPC's max size
			}

			float TotalLimit = (GetButtCrushSize(actor) + ((GetLimit * Potion_GetSizeMultiplier(actor)) * (1.0f + Ench_Aspect_GetPower(actor))));// / GameScale;
            //                ^ Add Butt Crush to base     ^          Multiply size with potions              ^ Aspect Of Giantess *'es it again

			if (get_max_scale(actor) < TotalLimit + Endless || get_max_scale(actor) > TotalLimit + Endless) {
				set_max_scale(actor, TotalLimit);
			}
		}
    }
}
