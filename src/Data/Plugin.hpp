#pragma once
// Keeps track of global state like is enabled

namespace GTS {

	class Plugin {
		public:
			static bool IsInRaceMenu();
			static bool Enabled();
			static bool InGame();
			static bool Ready();
			static bool Live();
			static bool AnyMenuOpen();
			static bool AnyWidgetMenuOpen();
			static void SetInGame(bool value);
			static bool OnMainThread();
			static void SetOnMainThread(bool value);


		private:
			[[nodiscard]] static Plugin& GetSingleton();
			std::atomic_bool enabled = std::atomic_bool(true);
			std::atomic_bool ingame = std::atomic_bool(false);
			std::atomic_bool onmainthread = std::atomic_bool(false);
	};
}
