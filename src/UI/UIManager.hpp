#pragma once
namespace GTSUI {

	class UIManager {

		private:

		UIManager(const UIManager&) = delete;
		UIManager& operator=(const UIManager&) = delete;

		bool Initialized = false;
		UIManager() = default;
		~UIManager() = default;

		public:

		static UIManager& GetSingleton() {
			static UIManager _Instance;
			return _Instance;
		}

		void Init();
		void Render();

	};



}
