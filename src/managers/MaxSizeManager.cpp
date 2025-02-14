#include "managers/MaxSizeManager.hpp"
#include "managers/ai/aifunctions.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "data/runtime.hpp"
#include "colliders/RE.hpp"
#include "rays/raycast.hpp"
#include "scale/scale.hpp"
#include "UI/DebugAPI.hpp"
#include "utils/debug.hpp"
#include "utils/av.hpp"
#include "profiler.hpp"
#include "events.hpp"
#include "spring.hpp"
#include "timer.hpp"

namespace {
    float get_endless_height(Actor* actor) {
		float endless = 0.0f;
		if (Runtime::HasPerk(actor, "ColossalGrowth")) {
			endless = 99999999.0f;
		}
		return endless;
	}

    float get_default_size_limit(float NaturalScale, float BaseLimit) { // default size limit for everyone
        float size_calc = NaturalScale + ((BaseLimit - 1.0f) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f, 99999999.0f);

        return GetLimit;
    }

    float get_mass_based_limit(Actor* actor, float NaturalScale) { // get mass based size limit for Player if using Mass Based mode
        float low_limit = get_endless_height(actor);
        if (low_limit <= 0.0f) {
            low_limit = Runtime::GetFloat("sizeLimit"); // Cap max size through normal size rules
            // Else max possible size is unlimited
        }
        float size_calc = NaturalScale + (Runtime::GetFloat("GtsMassBasedSize") * NaturalScale);
        float GetLimit = std::clamp(size_calc, NaturalScale, low_limit);

        return GetLimit;
    }

    float get_follower_size_limit(float NaturalScale, float FollowerLimit) { // Self explanatory
        float size_calc = NaturalScale + ((FollowerLimit) * NaturalScale);
        float GetLimit = std::clamp(size_calc, 1.0f * FollowerLimit, 99999999.0f);

        return GetLimit;
    }

    float get_npc_size_limit(float NaturalScale, float NPCLimit) { // get non-follower size limit
        float size_calc = NaturalScale + ((NPCLimit - 1.0f) * NaturalScale);
		float GetLimit = std::clamp(size_calc, 1.0f * NPCLimit, 99999999.0f);

        return GetLimit;
    }
}

namespace GTS {
    void UpdateMaxScale() {
        auto profiler = Profilers::Profile("SizeManager: Update");
		for (auto actor: find_actors()) {
			// 2023 + 2024: TODO: move away from polling
			float Endless = 0.0f;
			if (actor->formID == 0x14) {
				Endless = get_endless_height(actor);
			}

            float NaturalScale = get_natural_scale(actor, true);
            float QuestStage = Runtime::GetStage("MainQuest");

			float BaseLimit = Runtime::GetFloatOr("sizeLimit", 1.0f);
            float NPCLimit = Runtime::GetFloatOr("NPCSizeLimit", 1.0f); // 0 by default
			float IsMassBased = Runtime::GetFloatOr("SelectedSizeFormula", 0.0f); // Should DLL use mass based formula for Player?
			float FollowerLimit = Runtime::GetFloatOr("FollowersSizeLimit", 1.0f); // 0 by default

            float GetLimit = get_default_size_limit(NaturalScale, BaseLimit); // Default size limit
			
			if (actor->formID == 0x14 && IsMassBased >= 1.0f) { 
				GetLimit = get_mass_based_limit(actor, NaturalScale); // Apply Player Mass-Based max size
			} else if (QuestStage > 100 && FollowerLimit > 0.0f && FollowerLimit != 1.0f && actor->formID != 0x14 && IsTeammate(actor)) { 
				GetLimit = get_follower_size_limit(NaturalScale, FollowerLimit); // Apply Follower Max Size
			} else if (QuestStage > 100 && NPCLimit > 0.0f && NPCLimit != 1.0f && actor->formID != 0x14 && !IsTeammate(actor)) { 
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
