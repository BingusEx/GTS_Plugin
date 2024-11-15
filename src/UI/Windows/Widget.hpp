#pragma once
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "UI/UIWindow.hpp"

using namespace RE;
using namespace SKSE;

namespace Gts {

	class WidgetWindow : UIWindow {

		public:

		void Update() override;
		void Init() override;

		WidgetWindow() noexcept = default;
		~WidgetWindow() noexcept = default;

		WidgetWindow(const WidgetWindow&) = delete;
		WidgetWindow(WidgetWindow&&) = delete;
		WidgetWindow& operator=(const WidgetWindow&) = delete;
		WidgetWindow& operator=(WidgetWindow&&) = delete;

		//virtual void Update();

	};

}