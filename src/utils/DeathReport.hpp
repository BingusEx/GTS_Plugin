#pragma once



using namespace std;
using namespace SKSE;

using namespace GTS;

namespace GTS {
	std::string_view GetDeathNodeName(DamageSource cause);
	void ReportDeath(Actor* giant, Actor* tiny, DamageSource cause);
}