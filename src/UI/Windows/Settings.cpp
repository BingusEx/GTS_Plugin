#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

#include "UI/UIWindowManager.hpp"
#include "UI/UIWindow.hpp"
#include "UI/Windows/Settings.hpp"

using namespace RE;
using namespace SKSE;

namespace Gts {

	void SettingsWindow::Init()  {
		this->GetData()->Size = ImVec2(640,480);
		this->GetData()->Name = "Settings";
		this->GetData()->Show = true;
		UIWindowManager::GetSingleton().AddWindow(std::unique_ptr<UIWindow>(this));

	}

	void SettingsWindow::Update(){
		//logger::info("PRESENT");

		ImGui::Begin(this->GetData()->Name.c_str());
		ImGui::SetWindowSize(this->GetData()->Size);
		ImGui::End();
	}

}