#pragma once
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

using namespace RE;
using namespace SKSE;

namespace Gts {

	class UIWindow {

		public:

		struct WindowData {
			bool Show = false;
			bool Focus = false;
			ImVec2 Size = ImVec2(100.f, 100.f);
			ImVec2 Position = ImVec2(720.0f, 1280.f);
			std::string Name = "Window";
		};

		std::shared_ptr<UIWindow> getSharedPtr() {
			// Creates a shared_ptr from this
		}

		UIWindow(WindowData a_data) {
			Data = a_data;
		}

		UIWindow() = default;
		virtual ~UIWindow() noexcept = default;

		UIWindow(const UIWindow&) = delete;
		UIWindow(UIWindow&&) = delete;
		UIWindow& operator=(const UIWindow&) = delete;
		UIWindow& operator=(UIWindow&&) = delete;

		virtual void Update() = 0;
		virtual void Init() = 0;

		WindowData* GetData() {
			return &Data;
		}

		private:
		WindowData Data;
	};

}