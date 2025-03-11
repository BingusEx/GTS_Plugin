#include "Data/Plugin.hpp"

#include "UI/UIManager.hpp"

namespace GTS {

	bool Plugin::IsInRaceMenu() {
		auto ui = UI::GetSingleton();
		if (ui->GetMenu(RaceSexMenu::MENU_NAME)) {
			return true; // Disallow to do animations in RaceMenu, not more than that. Allows scaling and such.
		}
		return false;
	}

	bool Plugin::Enabled() {
		return Plugin::GetSingleton().enabled.load();
	}

	bool Plugin::InGame() {
		auto ui = RE::UI::GetSingleton();
		if (!ui) {
			return false;
		}
		if (ui->IsMenuOpen(MainMenu::MENU_NAME)) {
			return false;
		}

		return Plugin::GetSingleton().ingame.load();
	}

	void Plugin::SetInGame(bool value) {
		Plugin::GetSingleton().ingame.store(value);
	}

	bool Plugin::Ready() {
		if (InGame()) {
			// We are not loading or in the mainmenu
			auto player_char = RE::PlayerCharacter::GetSingleton();
			if (player_char) {
				if (player_char->Is3DLoaded()) {
					// Player is loaded
					return true;
				}
			}
		}
		return false;
	}

	bool Plugin::Live() {
        if (Ready()) {
            auto ui = RE::UI::GetSingleton();

            if (UIManager::MenuOpen() && GTS::UIManager::GamePaused) {
                return false;
            }

            if (ui->GameIsPaused() && !IsInRaceMenu()) { // We don't want to pause dll scaling in racemenu
                return false;
            }

            return true;

        }
        return false;
    }

	bool Plugin::AnyMenuOpen() {

		//Static const means the list doesnt get recreated Each call. So no perf impact here.
		static const std::vector<std::string_view> Menus = {
			RE::CraftingMenu::MENU_NAME,
			RE::BarterMenu::MENU_NAME,
			RE::BookMenu::MENU_NAME,
			RE::ContainerMenu::MENU_NAME,
			RE::FavoritesMenu::MENU_NAME,
			RE::GiftMenu::MENU_NAME,
			RE::InventoryMenu::MENU_NAME,
			RE::JournalMenu::MENU_NAME,
			RE::LevelUpMenu::MENU_NAME,
			RE::LockpickingMenu::MENU_NAME,
			RE::MagicMenu::MENU_NAME,
			RE::MapMenu::MENU_NAME,
			RE::MessageBoxMenu::MENU_NAME,
			RE::RaceSexMenu::MENU_NAME,
			RE::SleepWaitMenu::MENU_NAME,
			RE::StatsMenu::MENU_NAME,
			RE::TrainingMenu::MENU_NAME,
			RE::TutorialMenu::MENU_NAME,
			RE::TweenMenu::MENU_NAME,
			RE::FaderMenu::MENU_NAME
		};

		if (Plugin::Ready()) {
			//Looping Through the list Is O(n) (almost) no perf impact here.
			auto ui = RE::UI::GetSingleton();
			for (const auto& Menu : Menus) {
				if (ui->IsMenuOpen(Menu)) {
					//log::debug("Menu is open: {}", Menu);
					return true;
				}
			}
		}
		return false;
	}

	bool Plugin::AnyWidgetMenuOpen() {

		//Static const means the list doesnt get recreated Each call. So no perf impact here.
		static const std::vector<std::string_view> Menus = {
			RE::CraftingMenu::MENU_NAME,
			RE::BarterMenu::MENU_NAME,
			RE::BookMenu::MENU_NAME,
			RE::ContainerMenu::MENU_NAME,
			RE::GiftMenu::MENU_NAME,
			RE::InventoryMenu::MENU_NAME,
			RE::JournalMenu::MENU_NAME,
			RE::LevelUpMenu::MENU_NAME,
			RE::LockpickingMenu::MENU_NAME,
			RE::MagicMenu::MENU_NAME,
			RE::MapMenu::MENU_NAME,
			RE::MessageBoxMenu::MENU_NAME,
			RE::RaceSexMenu::MENU_NAME,
			RE::StatsMenu::MENU_NAME,
			RE::TrainingMenu::MENU_NAME,
			RE::TutorialMenu::MENU_NAME,
			RE::TweenMenu::MENU_NAME,
		};

		if (Plugin::Ready()) {
			//Looping Through the list Is O(n) (almost) no perf impact here.
			auto ui = RE::UI::GetSingleton();
			for (const auto& Menu : Menus) {
				if (ui->IsMenuOpen(Menu)) {
					//log::debug("Menu is open: {}", Menu);
					return true;
				}
			}
		}
		return false;
	}

	bool Plugin::OnMainThread() {
		return Plugin::GetSingleton().onmainthread.load();
	}

	void Plugin::SetOnMainThread(bool value) {
		Plugin::GetSingleton().onmainthread.store(value);
	}

	Plugin& Plugin::GetSingleton() {
		static Plugin instance;
		return instance;
	}
}
