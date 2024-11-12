#include "scale/scale.hpp"
#include "managers/GtsManager.hpp"
#include "managers/GtsSizeManager.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "data/transient.hpp"
#include "data/runtime.hpp"
#include "scale/height.hpp"
#include "timer.hpp"

using namespace Gts;

namespace {
	const float EPS = 1e-4f;
}

namespace Gts {

	void set_target_scale(Actor& actor, float scale) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			float natural_scale = get_natural_scale(&actor, true);
			float target_scale = actor_data->target_scale * natural_scale;

			scale /= natural_scale;

			if (scale < (actor_data->max_scale + EPS)) { // If new value is below max: allow it
				actor_data->target_scale = scale;
			} else if (target_scale < (actor_data->max_scale - EPS)) { // If we are below max currently and we are trying to scale over max: make it max
				actor_data->target_scale = actor_data->max_scale / natural_scale;
			} else {
				// If we are over max: forbid it
			}
		}
	}
	void set_target_scale(Actor* actor, float scale) {
		if (actor) {
			Actor& a = *actor;
			set_target_scale(a, scale);
		}
	}

	float get_target_scale(Actor& actor) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			return actor_data->target_scale * get_natural_scale(&actor, true);
		} else {
			return -1.0f;
		}
	}
	float get_target_scale(Actor* actor) {
		if (actor) {
			Actor& a = *actor;
			return get_target_scale(a);
		} else {
			return -1.0f;
		}
	}

	void mod_target_scale(Actor& actor, float amt) {
		auto profiler = Profilers::Profile("Scale: ModTargetScale");
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			float natural_scale = get_natural_scale(&actor, true);
			float target_scale = actor_data->target_scale * natural_scale;

			amt /= natural_scale;

			if (amt - EPS < 0.0f) { // If neative change always: allow
				actor_data->target_scale += amt;
			} else if (target_scale + amt < (actor_data->max_scale + EPS)) { // If change results is below max: allow it
				actor_data->target_scale += amt;
			} else if (target_scale < (actor_data->max_scale - EPS)) { // If we are currently below max and we are scaling above max: make it max
				set_target_scale(actor, actor_data->max_scale);
			} else { // if we are over max then don't allow it
			}
		}
	}
	void mod_target_scale(Actor* actor, float amt) {
		if (actor) {
			mod_target_scale(*actor, amt);
		}
	}

	void set_max_scale(Actor& actor, float scale) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			actor_data->max_scale = scale;
		}
	}
	void set_max_scale(Actor* actor, float scale) {
		if (actor) {
			set_max_scale(*actor, scale);
		}
	}

	float get_max_scale(Actor& actor) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			return actor_data->max_scale;
		}
		return -1.0f;
	}
	float get_max_scale(Actor* actor) {
		if (actor) {
			return get_max_scale(*actor);
		}
		return -1.0f;
	}

	void mod_max_scale(Actor& actor, float amt) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			actor_data->max_scale += amt;
		}
	}
	void mod_max_scale(Actor* actor, float amt) {
		if (actor) {
			mod_max_scale(*actor, amt);
		}
	}

	float get_visual_scale(Actor& actor) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			return actor_data->visual_scale * get_natural_scale(&actor, true);
		}
		return -1.0f;
	}
	float get_visual_scale(Actor* actor) {
		if (actor) {
			return get_visual_scale(*actor);
		}
		return -1.0f;
	}

	float get_natural_scale(Actor& actor, bool game_scale) {
		auto actor_data = Transient::GetSingleton().GetData(&actor);
		if (actor_data) {
			//static Timer timer = Timer(5.0f);
		    float initialScale = GetInitialScale(&actor);
			/*if (actor.formID == 0x14 && timer.ShouldRunFrame()) {
				log::info("Initial Scale: {}", initialScale);
				log::info("Other Scales: {}", actor_data->otherScales);
				log::info("gamescale: {}", game_getactorscale(&actor));
				log::info("npcparentnode: {}", get_npcparentnode_scale(&actor));
				log::info("GetScale: {}", actor.GetScale());
			}*/
			float result = actor_data->otherScales * initialScale;
			if (game_scale) {
				result *= game_getactorscale(&actor);
			}
			return result;
			// otherScales reads RaceMenu scale
		}
		return 1.0f;
	}

	float get_natural_scale(Actor* actor, bool game_scale) {
		if (actor) {
			return get_natural_scale(*actor, game_scale);
		}
		return 1.0f;
	}

	float get_natural_scale(Actor* actor) {
		if (actor) {
			return get_natural_scale(*actor, false);
		}
		return 1.0f;
	}

	float get_neutral_scale(Actor* actor) {
		return 1.0f;
	}

	float get_giantess_scale(Actor& actor) {
		auto actor_data = Persistent::GetSingleton().GetData(&actor);
		if (actor_data) {
			float result = actor_data->visual_scale * get_natural_scale(&actor, true);
			// Sadly had to add natural scale to it so it will respect GetScale * RaceMenu alterations
			return result;
		}
		return -1.0f;
	}
	float get_giantess_scale(Actor* actor) {
		if (actor) {
			return get_giantess_scale(*actor);
		}
		return -1.0f;
	}

}
