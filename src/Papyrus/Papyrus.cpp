#include "papyrus/papyrus.hpp"
#include "papyrus/plugin.hpp"
#include "papyrus/scale.hpp"
#include "papyrus/height.hpp"
#include "papyrus/modevents.hpp"
#include "papyrus/camera.hpp"
#include "papyrus/totalcontrol.hpp"



using namespace GTS;
using namespace RE::BSScript;

namespace GTS {

	bool register_papyrus(IVirtualMachine* vm) {
		register_papyrus_plugin(vm);
		register_papyrus_scale(vm);
		register_papyrus_height(vm);
		register_papyrus_events(vm);
		register_papyrus_camera(vm);
		register_total_control(vm);
		return true;
	}

}
