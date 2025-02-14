#pragma once

#include "events.hpp"

using namespace std;
using namespace SKSE;

using namespace GTS;

namespace GTS {
    enum class ChestType {
		BossChest,
		NormalChest,
		MiscChest,
	};

    TESContainer* FilterChests(TESForm* form, ChestType type);

    void DistributeChestItems();

    void AddItemToChests(TESForm* Chest);

    std::vector<TESForm*> FindAllChests();
   
	std::vector<TESLevItem*> CalculateItemProbability(ChestType type);
    std::vector<TESLevItem*> SelectItemsFromPool(ChestType type); 
}