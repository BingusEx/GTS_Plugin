#pragma once
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "UI/UIWindow.hpp"
#include "Settings.hpp"

using namespace RE;
using namespace SKSE;

namespace Gts {

	class SettingsWindow : UIWindow {

		public:

		void Update() override;
		void Init() override;
		
		SettingsWindow() noexcept = default;
		~SettingsWindow() noexcept = default;

		SettingsWindow(const SettingsWindow&) = delete;
		SettingsWindow(SettingsWindow&&) = delete;
		SettingsWindow& operator=(const SettingsWindow&) = delete;
		SettingsWindow& operator=(SettingsWindow&&) = delete;

		//virtual void Update();

	};

}