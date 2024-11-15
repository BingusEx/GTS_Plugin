#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "UI/UIWindowManager.hpp"
#include "UI/UIWindow.hpp"
#include "UI/Windows/Widget.hpp"

using namespace RE;
using namespace SKSE;

namespace Gts {

	void WidgetWindow::Init() {
		this->GetData()->Size = ImVec2(300, 400);
		this->GetData()->Position = ImVec2(500, 500);
		this->GetData()->Name = "Widget";
		this->GetData()->Show = true;
		UIWindowManager::GetSingleton().AddWindow(std::unique_ptr<UIWindow>(this));

	}

	void WidgetWindow::Update() {
		//logger::info("PRESENT");

		ImGui::Begin(this->GetData()->Name.c_str());
		ImGui::SetWindowSize(this->GetData()->Size);
		ImGui::SetWindowPos(GetData()->Position);
		ImGui::End();
	}

}