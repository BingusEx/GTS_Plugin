#pragma once

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "UIWindow.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;


namespace Gts {

	class UIWindowManager {

		private:

		UIWindowManager(const UIWindowManager&) = delete;
		UIWindowManager& operator=(const UIWindowManager&) = delete;

		UIWindowManager() {
			logger::info("UIWindowManager Instantiated");
		}

		~UIWindowManager() {
			logger::info("UIWindowManager Destroyed");
		}

		std::vector<std::unique_ptr<UIWindow>> imWindows;

		public:

		static UIWindowManager& GetSingleton() {
			static UIWindowManager _Instance;
			return _Instance;
		}

		
		void AddWindow(std::unique_ptr<UIWindow> a_window);
		void Update();
		bool HasWindows();


		//void SetDimensions(float a_offsetX /*= 0.f*/, float a_offsetY /*= 0.f*/, float a_sizeX /*= -1.f*/, float a_sizeY /*= -1.f*/, WindowAlignment a_alignment /*= WindowAlignment::kTopLeft*/, ImVec2 a_sizeMin /*= ImVec2(0, 0)*/, ImVec2 a_sizeMax /*= ImVec2(0, 0)*/, ImGuiCond_ a_cond /*= ImGuiCond_FirstUseEver*/);

	};

}
