#pragma once
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

using namespace SKSE;
using namespace RE;

namespace Gts {

	class UIRenderer {

		private:

		UIRenderer(const UIRenderer&) = delete;
		UIRenderer& operator=(const UIRenderer&) = delete;

		bool Initialized = false;

		UIRenderer() {
			logger::info("UIManager Instantiated");
		}

		~UIRenderer() {
			//TODO
			logger::info("UIManager Destroyed");
		}

		public:

		static UIRenderer& GetSingleton() {
			static UIRenderer _Instance;
			return _Instance;
		}

		void Init();
		void Render();

	};



}
