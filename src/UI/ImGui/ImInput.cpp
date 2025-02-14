#include "UI/ImGui/ImInput.hpp"
#include "ImWindowManager.hpp"

namespace GTS {

	void ImInput::ProcessInputEventQueue() {

		std::unique_lock<std::shared_mutex> mutex(InputMutex);

		ImGuiIO& io = ImGui::GetIO();

		for (auto& event : KeyEventQueue) {

			if (event.eventType == RE::INPUT_EVENT_TYPE::kChar) {
				io.AddInputCharacter(event.keyCode);
				continue;
			}

			if (event.device == RE::INPUT_DEVICE::kMouse) {
				
				if (event.keyCode > 7) {  // middle scroll
					io.AddMouseWheelEvent(0, event.value * (event.keyCode == 8 ? 1 : -1));
				}
				else {
					event.keyCode = std::min<uint32_t>(event.keyCode, 5);
					io.AddMouseButtonEvent(event.keyCode, event.IsPressed());
				}
			}

			if (event.device == RE::INPUT_DEVICE::kKeyboard) {

				uint32_t key = DIKToVK(event.keyCode);

				logger::debug("VK Keycode", event.keyCode, key);

				if (key == event.keyCode) {
					key = MapVirtualKeyEx(event.keyCode, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
				}

				if (!event.IsPressed()) {

					if (auto MainWindow = ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
						if (key == VK_ESCAPE && MainWindow->Show) {
							MainWindow->Show = false;
						}
					}

				}

				io.AddKeyEvent(VirtualKeyToImGuiKey(key), event.IsPressed());

				if (key == VK_LCONTROL || key == VK_RCONTROL) {
					io.AddKeyEvent(ImGuiMod_Ctrl, event.IsPressed());
				}
				else if (key == VK_LSHIFT || key == VK_RSHIFT) {
					io.AddKeyEvent(ImGuiMod_Shift, event.IsPressed());
				}
				else if (key == VK_LMENU || key == VK_RMENU) {
					io.AddKeyEvent(ImGuiMod_Alt, event.IsPressed());
				}
			}
		}

		KeyEventQueue.clear();
	}
}
