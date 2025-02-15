#pragma once

#include "UI/DearImGui/imgui.h"

namespace GTS {

    #define IM_VK_KEYPAD_ENTER (VK_RETURN + 256)

	static const ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam) {

		switch (wParam) {
		case VK_TAB:
			return ImGuiKey_Tab;
		case VK_LEFT:
			return ImGuiKey_LeftArrow;
		case VK_RIGHT:
			return ImGuiKey_RightArrow;
		case VK_UP:
			return ImGuiKey_UpArrow;
		case VK_DOWN:
			return ImGuiKey_DownArrow;
		case VK_PRIOR:
			return ImGuiKey_PageUp;
		case VK_NEXT:
			return ImGuiKey_PageDown;
		case VK_HOME:
			return ImGuiKey_Home;
		case VK_END:
			return ImGuiKey_End;
		case VK_INSERT:
			return ImGuiKey_Insert;
		case VK_DELETE:
			return ImGuiKey_Delete;
		case VK_BACK:
			return ImGuiKey_Backspace;
		case VK_SPACE:
			return ImGuiKey_Space;
		case VK_RETURN:
			return ImGuiKey_Enter;
		case VK_ESCAPE:
			return ImGuiKey_Escape;
		case VK_OEM_7:
			return ImGuiKey_Apostrophe;
		case VK_OEM_COMMA:
			return ImGuiKey_Comma;
		case VK_OEM_MINUS:
			return ImGuiKey_Minus;
		case VK_OEM_PERIOD:
			return ImGuiKey_Period;
		case VK_OEM_2:
			return ImGuiKey_Slash;
		case VK_OEM_1:
			return ImGuiKey_Semicolon;
		case VK_OEM_PLUS:
			return ImGuiKey_Equal;
		case VK_OEM_4:
			return ImGuiKey_LeftBracket;
		case VK_OEM_5:
			return ImGuiKey_Backslash;
		case VK_OEM_6:
			return ImGuiKey_RightBracket;
		case VK_OEM_3:
			return ImGuiKey_GraveAccent;
		case VK_CAPITAL:
			return ImGuiKey_CapsLock;
		case VK_SCROLL:
			return ImGuiKey_ScrollLock;
		case VK_NUMLOCK:
			return ImGuiKey_NumLock;
		case VK_SNAPSHOT:
			return ImGuiKey_PrintScreen;
		case VK_PAUSE:
			return ImGuiKey_Pause;
		case VK_NUMPAD0:
			return ImGuiKey_Keypad0;
		case VK_NUMPAD1:
			return ImGuiKey_Keypad1;
		case VK_NUMPAD2:
			return ImGuiKey_Keypad2;
		case VK_NUMPAD3:
			return ImGuiKey_Keypad3;
		case VK_NUMPAD4:
			return ImGuiKey_Keypad4;
		case VK_NUMPAD5:
			return ImGuiKey_Keypad5;
		case VK_NUMPAD6:
			return ImGuiKey_Keypad6;
		case VK_NUMPAD7:
			return ImGuiKey_Keypad7;
		case VK_NUMPAD8:
			return ImGuiKey_Keypad8;
		case VK_NUMPAD9:
			return ImGuiKey_Keypad9;
		case VK_DECIMAL:
			return ImGuiKey_KeypadDecimal;
		case VK_DIVIDE:
			return ImGuiKey_KeypadDivide;
		case VK_MULTIPLY:
			return ImGuiKey_KeypadMultiply;
		case VK_SUBTRACT:
			return ImGuiKey_KeypadSubtract;
		case VK_ADD:
			return ImGuiKey_KeypadAdd;
		case IM_VK_KEYPAD_ENTER:
			return ImGuiKey_KeypadEnter;
		case VK_LSHIFT:
			return ImGuiKey_LeftShift;
		case VK_LCONTROL:
			return ImGuiKey_LeftCtrl;
		case VK_LMENU:
			return ImGuiKey_LeftAlt;
		case VK_LWIN:
			return ImGuiKey_LeftSuper;
		case VK_RSHIFT:
			return ImGuiKey_RightShift;
		case VK_RCONTROL:
			return ImGuiKey_RightCtrl;
		case VK_RMENU:
			return ImGuiKey_RightAlt;
		case VK_RWIN:
			return ImGuiKey_RightSuper;
		case VK_APPS:
			return ImGuiKey_Menu;
		case '0':
			return ImGuiKey_0;
		case '1':
			return ImGuiKey_1;
		case '2':
			return ImGuiKey_2;
		case '3':
			return ImGuiKey_3;
		case '4':
			return ImGuiKey_4;
		case '5':
			return ImGuiKey_5;
		case '6':
			return ImGuiKey_6;
		case '7':
			return ImGuiKey_7;
		case '8':
			return ImGuiKey_8;
		case '9':
			return ImGuiKey_9;
		case 'A':
			return ImGuiKey_A;
		case 'B':
			return ImGuiKey_B;
		case 'C':
			return ImGuiKey_C;
		case 'D':
			return ImGuiKey_D;
		case 'E':
			return ImGuiKey_E;
		case 'F':
			return ImGuiKey_F;
		case 'G':
			return ImGuiKey_G;
		case 'H':
			return ImGuiKey_H;
		case 'I':
			return ImGuiKey_I;
		case 'J':
			return ImGuiKey_J;
		case 'K':
			return ImGuiKey_K;
		case 'L':
			return ImGuiKey_L;
		case 'M':
			return ImGuiKey_M;
		case 'N':
			return ImGuiKey_N;
		case 'O':
			return ImGuiKey_O;
		case 'P':
			return ImGuiKey_P;
		case 'Q':
			return ImGuiKey_Q;
		case 'R':
			return ImGuiKey_R;
		case 'S':
			return ImGuiKey_S;
		case 'T':
			return ImGuiKey_T;
		case 'U':
			return ImGuiKey_U;
		case 'V':
			return ImGuiKey_V;
		case 'W':
			return ImGuiKey_W;
		case 'X':
			return ImGuiKey_X;
		case 'Y':
			return ImGuiKey_Y;
		case 'Z':
			return ImGuiKey_Z;
		case VK_F1:
			return ImGuiKey_F1;
		case VK_F2:
			return ImGuiKey_F2;
		case VK_F3:
			return ImGuiKey_F3;
		case VK_F4:
			return ImGuiKey_F4;
		case VK_F5:
			return ImGuiKey_F5;
		case VK_F6:
			return ImGuiKey_F6;
		case VK_F7:
			return ImGuiKey_F7;
		case VK_F8:
			return ImGuiKey_F8;
		case VK_F9:
			return ImGuiKey_F9;
		case VK_F10:
			return ImGuiKey_F10;
		case VK_F11:
			return ImGuiKey_F11;
		case VK_F12:
			return ImGuiKey_F12;
        case VK_F13:
			return ImGuiKey_F13;
		case VK_F14:
			return ImGuiKey_F14;
		case VK_F15:
			return ImGuiKey_F15;
		case VK_F16:
			return ImGuiKey_F16;
		case VK_F17:
			return ImGuiKey_F17;
		case VK_F18:
			return ImGuiKey_F18;
		case VK_F19:
			return ImGuiKey_F19;
		case VK_F20:
			return ImGuiKey_F20;
		case VK_F21:
			return ImGuiKey_F21;
		case VK_F22:
			return ImGuiKey_F22;
		case VK_F23:
			return ImGuiKey_F23;
		case VK_F24:
			return ImGuiKey_F24;
		default:
			return ImGuiKey_None;
		}
	}

	// This lookup table converts ImGui keys to the string representation of DirectInput keys,
	// with the "DIK_" prefix removed.
	//This is intended to be used with the existing way Inputmanager parses keys.
	//Which uses a dinput LUT from the wine project.
	//I mean we could just use dinput directly at this point...
	//But if it ain't broke don't fix it. Even if its a roundabout way of doing it...
	static const std::unordered_map<ImGuiKey, std::string> ImGuiKeyToDIKString = {
		// Basic editing / navigation keys
		{ ImGuiKey_Escape,      "ESCAPE" },
		{ ImGuiKey_Tab,         "TAB" },
		{ ImGuiKey_Backspace,   "BACK" },
		{ ImGuiKey_Space,       "SPACE" },
		{ ImGuiKey_Enter,       "RETURN" },
		{ ImGuiKey_LeftArrow,   "LEFT" },
		{ ImGuiKey_RightArrow,  "RIGHT" },
		{ ImGuiKey_UpArrow,     "UP" },
		{ ImGuiKey_DownArrow,   "DOWN" },
		{ ImGuiKey_PageUp,      "PAGEUP" },
		{ ImGuiKey_PageDown,    "PAGEDOWN" },
		{ ImGuiKey_Home,        "HOME" },
		{ ImGuiKey_End,         "END" },
		{ ImGuiKey_Insert,      "INSERT" },
		{ ImGuiKey_Delete,      "DELETE" },

		// Letters (A-Z)
		{ ImGuiKey_A,           "A" },
		{ ImGuiKey_B,           "B" },
		{ ImGuiKey_C,           "C" },
		{ ImGuiKey_D,           "D" },
		{ ImGuiKey_E,           "E" },
		{ ImGuiKey_F,           "F" },
		{ ImGuiKey_G,           "G" },
		{ ImGuiKey_H,           "H" },
		{ ImGuiKey_I,           "I" },
		{ ImGuiKey_J,           "J" },
		{ ImGuiKey_K,           "K" },
		{ ImGuiKey_L,           "L" },
		{ ImGuiKey_M,           "M" },
		{ ImGuiKey_N,           "N" },
		{ ImGuiKey_O,           "O" },
		{ ImGuiKey_P,           "P" },
		{ ImGuiKey_Q,           "Q" },
		{ ImGuiKey_R,           "R" },
		{ ImGuiKey_S,           "S" },
		{ ImGuiKey_T,           "T" },
		{ ImGuiKey_U,           "U" },
		{ ImGuiKey_V,           "V" },
		{ ImGuiKey_W,           "W" },
		{ ImGuiKey_X,           "X" },
		{ ImGuiKey_Y,           "Y" },
		{ ImGuiKey_Z,           "Z" },

		// Digits (0-9)
		{ ImGuiKey_0,           "0" },
		{ ImGuiKey_1,           "1" },
		{ ImGuiKey_2,           "2" },
		{ ImGuiKey_3,           "3" },
		{ ImGuiKey_4,           "4" },
		{ ImGuiKey_5,           "5" },
		{ ImGuiKey_6,           "6" },
		{ ImGuiKey_7,           "7" },
		{ ImGuiKey_8,           "8" },
		{ ImGuiKey_9,           "9" },

		// Function keys (F1-F24)
		{ ImGuiKey_F1,          "F1" },
		{ ImGuiKey_F2,          "F2" },
		{ ImGuiKey_F3,          "F3" },
		{ ImGuiKey_F4,          "F4" },
		{ ImGuiKey_F5,          "F5" },
		{ ImGuiKey_F6,          "F6" },
		{ ImGuiKey_F7,          "F7" },
		{ ImGuiKey_F8,          "F8" },
		{ ImGuiKey_F9,          "F9" },
		{ ImGuiKey_F10,         "F10" },
		{ ImGuiKey_F11,         "F11" },
		{ ImGuiKey_F12,         "F12" },
		{ ImGuiKey_F13,         "F13" },
		{ ImGuiKey_F14,         "F14" },
		{ ImGuiKey_F15,         "F15" },
		{ ImGuiKey_F16,         "F16" },
		{ ImGuiKey_F17,         "F17" },
		{ ImGuiKey_F18,         "F18" },
		{ ImGuiKey_F19,         "F19" },
		{ ImGuiKey_F20,         "F20" },
		{ ImGuiKey_F21,         "F21" },
		{ ImGuiKey_F22,         "F22" },
		{ ImGuiKey_F23,         "F23" },
		{ ImGuiKey_F24,         "F24" },

		// Keypad keys
		{ ImGuiKey_Keypad0,         "NUMPAD0" },
		{ ImGuiKey_Keypad1,         "NUMPAD1" },
		{ ImGuiKey_Keypad2,         "NUMPAD2" },
		{ ImGuiKey_Keypad3,         "NUMPAD3" },
		{ ImGuiKey_Keypad4,         "NUMPAD4" },
		{ ImGuiKey_Keypad5,         "NUMPAD5" },
		{ ImGuiKey_Keypad6,         "NUMPAD6" },
		{ ImGuiKey_Keypad7,         "NUMPAD7" },
		{ ImGuiKey_Keypad8,         "NUMPAD8" },
		{ ImGuiKey_Keypad9,         "NUMPAD9" },
		{ ImGuiKey_KeypadDecimal,   "NUMPAD_DECIMAL" },
		{ ImGuiKey_KeypadDivide,    "NUMPAD_DIVIDE" },
		{ ImGuiKey_KeypadMultiply,  "NUMPAD_MULTIPLY" },
		{ ImGuiKey_KeypadSubtract,  "NUMPAD_SUBTRACT" },
		{ ImGuiKey_KeypadAdd,       "NUMPAD_ADD" },

		// Modifier keys
		{ ImGuiKey_LeftShift,   "LSHIFT" },
		{ ImGuiKey_LeftCtrl,    "LCONTROL" },
		{ ImGuiKey_LeftAlt,     "LALT" },
		//{ ImGuiKey_LeftSuper,   "LWIN" },
		{ ImGuiKey_RightShift,  "RSHIFT" },
		{ ImGuiKey_RightCtrl,   "RCONTROL" },
		{ ImGuiKey_RightAlt,    "RALT" },
		//{ ImGuiKey_RightSuper,  "RWIN" },

		// Application keys
		//{ ImGuiKey_AppBack,     "BROWSER_BACK" },
		//{ ImGuiKey_AppForward,  "BROWSER_FORWARD" },

		{ ImGuiKey_MouseLeft,    "LMB" },  // DIK_ convention: Left Mouse Button
		{ ImGuiKey_MouseRight,   "RMB" },  // Right Mouse Button
		{ ImGuiKey_MouseMiddle,  "MOUSE3" },  // Middle Mouse Button (Scroll Wheel Click)
		{ ImGuiKey_MouseX1,      "MOUSE4" },  // Extra Mouse Button 1 (Backward)
		{ ImGuiKey_MouseX2,      "MOUSE5" }   // Extra Mouse Button 2 (Forward)
	};

	static const uint32_t DIKToVK(uint32_t DIK) {
		switch (DIK) {
			case DIK_LEFTARROW:
				return VK_LEFT;
			case DIK_RIGHTARROW:
				return VK_RIGHT;
			case DIK_UPARROW:
				return VK_UP;
			case DIK_DOWNARROW:
				return VK_DOWN;
			case DIK_DELETE:
				return VK_DELETE;
			case DIK_END:
				return VK_END;
			case DIK_HOME:
				return VK_HOME;  // pos1
			case DIK_PRIOR:
				return VK_PRIOR;  // page up
			case DIK_NEXT:
				return VK_NEXT;  // page down
			case DIK_INSERT:
				return VK_INSERT;
			case DIK_NUMPAD0:
				return VK_NUMPAD0;
			case DIK_NUMPAD1:
				return VK_NUMPAD1;
			case DIK_NUMPAD2:
				return VK_NUMPAD2;
			case DIK_NUMPAD3:
				return VK_NUMPAD3;
			case DIK_NUMPAD4:
				return VK_NUMPAD4;
			case DIK_NUMPAD5:
				return VK_NUMPAD5;
			case DIK_NUMPAD6:
				return VK_NUMPAD6;
			case DIK_NUMPAD7:
				return VK_NUMPAD7;
			case DIK_NUMPAD8:
				return VK_NUMPAD8;
			case DIK_NUMPAD9:
				return VK_NUMPAD9;
			case DIK_DECIMAL:
				return VK_DECIMAL;
			case DIK_NUMPADENTER:
				return IM_VK_KEYPAD_ENTER;
			case DIK_RMENU:
				return VK_RMENU;  // right alt
			case DIK_RCONTROL:
				return VK_RCONTROL;  // right control
			case DIK_LWIN:
				return VK_LWIN;  // left win
			case DIK_RWIN:
				return VK_RWIN;  // right win
			case DIK_APPS:
				return VK_APPS;
			default:
				return DIK;
		}
	}

	// Helper function: given an ImGuiKey, return the DirectInput key string (or an empty string if not found)
	inline static std::string GetDIKStringFromImGuiKey(ImGuiKey key) {
		auto it = ImGuiKeyToDIKString.find(key);
		if (it != ImGuiKeyToDIKString.end())
			return it->second;
		return "INVALID";
	}

	class ImInput {

		private:

		class CharEvent final : public RE::InputEvent {
			public:
			uint32_t keyCode;
		};

		struct KeyEvent {

			explicit KeyEvent(const RE::ButtonEvent* a_event) :
				keyCode(a_event->GetIDCode()),
				device(a_event->GetDevice()),
				eventType(a_event->GetEventType()),
				value(a_event->Value()),
				heldDownSecs(a_event->HeldDuration()) {}

			explicit KeyEvent(const CharEvent* a_event) :
				keyCode(a_event->keyCode),
				device(a_event->GetDevice()),
				eventType(a_event->GetEventType()) {}

			[[nodiscard]] constexpr bool IsPressed() const noexcept {
				return value > 0.0F;
			}
			[[nodiscard]] constexpr bool IsRepeating() const noexcept {
				return heldDownSecs > 0.0F;
			}
			[[nodiscard]] constexpr bool IsDown() const noexcept {
				return IsPressed() && (heldDownSecs == 0.0F);
			}
			[[nodiscard]] constexpr bool IsHeld() const noexcept {
				return IsPressed() && IsRepeating();
			}
			[[nodiscard]] constexpr bool IsUp() const noexcept {
				return (value == 0.0F) && IsRepeating();
			}

			uint32_t keyCode;
			RE::INPUT_DEVICE device;
			RE::INPUT_EVENT_TYPE eventType;
			float value = 0;
			float heldDownSecs = 0;
		};

		std::shared_mutex InputMutex;

		std::vector<KeyEvent> KeyEventQueue {};

		public:

		void ProcessInputEventQueue();

		void OnFocusLost() {
			std::unique_lock<std::shared_mutex> mutex(InputMutex);
			KeyEventQueue.clear();
		}

		void ProcessInputEvents(RE::InputEvent* const* a_events) {
			for (auto it = *a_events; it; it = it->next) {
				if (it->GetEventType() != RE::INPUT_EVENT_TYPE::kButton && it->GetEventType() != RE::INPUT_EVENT_TYPE::kChar)  // we do not care about non button or char events
					continue;

				auto event = it->GetEventType() == RE::INPUT_EVENT_TYPE::kButton ? KeyEvent(static_cast<RE::ButtonEvent*>(it)) : KeyEvent(static_cast<CharEvent*>(it));

				{
					std::unique_lock<std::shared_mutex> mutex(InputMutex);
					KeyEventQueue.emplace_back(event);
				}
			}
		}

	};

	// void UIManager::ProcessInputEvents(const RE::GTSInputEvent* const* a_events)
	// {
	// 	if (a_events) {
	// 		for (auto it = *a_events; it; it = it->next) {
	// 			switch (it->GetEventType()) {
	// 			case RE::INPUT_EVENT_TYPE::kButton:
	// 				{
	// 					const auto buttonEvent = static_cast<const RE::ButtonEvent*>(it);
	// 					KeyEvent keyEvent(buttonEvent);

	// 					AddKeyEvent(keyEvent);
	// 					break;
	// 				}
	// 			case RE::INPUT_EVENT_TYPE::kChar:
	// 				{
	// 					const auto charEvent = static_cast<const CharEvent*>(it);
	// 					KeyEvent keyEvent(charEvent);

	// 					AddKeyEvent(keyEvent);

	// 					break;
	// 				}
	// 			}
	// 		}
	// 	}
	// }

	// RE::TESObjectREFR* UIManager::GetRefrToEvaluate()
	// {
	// 	const auto newConsoleRefr = Utils::GetConsoleRefr();
	// 	if (newConsoleRefr.get() != _consoleRefr) {
	// 		_consoleRefr = newConsoleRefr.get();
	// 		AnimationLog::GetSingleton().ClearAnimationLog();
	// 	}

	// 	if (_consoleRefr) {
	// 		return _consoleRefr;
	// 	}

	// 	return _refrToEvaluate;
	// }

	// void UIManager::SetRefrToEvaluate(RE::TESObjectREFR* a_refr)
	// {
	// 	_refrToEvaluate = a_refr;
	// 	AnimationLog::GetSingleton().ClearAnimationLog();
	// }

	// bool UIManager::ShouldConsumeInput() const
	// {
	// 	return _menusConsumingInput > 0;
	// }

	// void UIManager::AddInputConsumer()
	// {
	// 	if (++_menusConsumingInput > 0) {
	// 		ImGuiIO& io = ImGui::GetIO();
	// 		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	// 		io.MouseDrawCursor = true;
	// 	}
	// }

	// void UIManager::RemoveInputConsumer()
	// {
	// 	if (--_menusConsumingInput == 0) {
	// 		ImGuiIO& io = ImGui::GetIO();
	// 		io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
	// 		io.MouseDrawCursor = false;
	// 	}
	// }

	// void UIManager::AddKeyEvent(KeyEvent& a_keyEvent)
	// {
	// 	WriteLocker locker(_inputEventLock);

	// 	_keyEventQueue.emplace_back(a_keyEvent);
	// }

	// void UIManager::OnFocusLost()
	// {
	// 	_bFocusLost = true;
	// }

	// void UIManager::ProcessInputEventQueue()
	// {
	// 	_lastKeyPressed = 0;

	// 	WriteLocker locker(_inputEventLock);

	// 	ImGuiIO& io = ImGui::GetIO();

	// 	if (_bFocusLost) {
	// 		io.AddFocusEvent(false);
	// 		_keyEventQueue.clear();
	// 		_bShiftHeld = false;
	// 		_bCtrlHeld = false;
	// 		_bAltHeld = false;

	// 		_bFocusLost = false;
	// 	}

	// 	for (auto& event : _keyEventQueue) {
	// 		switch (event.eventType) {
	// 		case RE::INPUT_EVENT_TYPE::kChar:
	// 			{
	// 				io.AddInputCharacter(event.keyCode);
	// 				break;
	// 			}
	// 		case RE::INPUT_EVENT_TYPE::kButton:
	// 			{
	// 				uint32_t key = MapVirtualKeyEx(event.keyCode, MAPVK_VSC_TO_VK_EX, GetKeyboardLayout(0));
	// 				switch (event.keyCode) {
	// 				case DIK_LEFTARROW:
	// 					key = VK_LEFT;
	// 					break;
	// 				case DIK_RIGHTARROW:
	// 					key = VK_RIGHT;
	// 					break;
	// 				case DIK_UPARROW:
	// 					key = VK_UP;
	// 					break;
	// 				case DIK_DOWNARROW:
	// 					key = VK_DOWN;
	// 					break;
	// 				case DIK_DELETE:
	// 					key = VK_DELETE;
	// 					break;
	// 				case DIK_END:
	// 					key = VK_END;
	// 					break;
	// 				case DIK_HOME:
	// 					key = VK_HOME;
	// 					break;  // pos1
	// 				case DIK_PRIOR:
	// 					key = VK_PRIOR;
	// 					break;  // page up
	// 				case DIK_NEXT:
	// 					key = VK_NEXT;
	// 					break;  // page down
	// 				case DIK_INSERT:
	// 					key = VK_INSERT;
	// 					break;
	// 				case DIK_NUMPAD0:
	// 					key = VK_NUMPAD0;
	// 					break;
	// 				case DIK_NUMPAD1:
	// 					key = VK_NUMPAD1;
	// 					break;
	// 				case DIK_NUMPAD2:
	// 					key = VK_NUMPAD2;
	// 					break;
	// 				case DIK_NUMPAD3:
	// 					key = VK_NUMPAD3;
	// 					break;
	// 				case DIK_NUMPAD4:
	// 					key = VK_NUMPAD4;
	// 					break;
	// 				case DIK_NUMPAD5:
	// 					key = VK_NUMPAD5;
	// 					break;
	// 				case DIK_NUMPAD6:
	// 					key = VK_NUMPAD6;
	// 					break;
	// 				case DIK_NUMPAD7:
	// 					key = VK_NUMPAD7;
	// 					break;
	// 				case DIK_NUMPAD8:
	// 					key = VK_NUMPAD8;
	// 					break;
	// 				case DIK_NUMPAD9:
	// 					key = VK_NUMPAD9;
	// 					break;
	// 				case DIK_DECIMAL:
	// 					key = VK_DECIMAL;
	// 					break;
	// 				case DIK_NUMPADENTER:
	// 					key = IM_VK_KEYPAD_ENTER;
	// 					break;
	// 				case DIK_RMENU:
	// 					key = VK_RMENU;
	// 					break;  // right alt
	// 				case DIK_RCONTROL:
	// 					key = VK_RCONTROL;
	// 					break;  // right control
	// 				case DIK_LWIN:
	// 					key = VK_LWIN;
	// 					break;  // left win
	// 				case DIK_RWIN:
	// 					key = VK_RWIN;
	// 					break;  // right win
	// 				case DIK_APPS:
	// 					key = VK_APPS;
	// 					break;
	// 				default:
	// 					break;
	// 				}

	// 				const ImGuiKey imGuiKey = ImGui_ImplWin32_VirtualKeyToImGuiKey(key);

	// 				if (imGuiKey == ImGuiKey_LeftShift || imGuiKey == ImGuiKey_RightShift) {
	// 					_bShiftHeld = event.IsPressed();
	// 				} else if (imGuiKey == ImGuiKey_LeftCtrl || imGuiKey == ImGuiKey_RightCtrl) {
	// 					_bCtrlHeld = event.IsPressed();
	// 				} else if (imGuiKey == ImGuiKey_LeftAlt || imGuiKey == ImGuiKey_RightAlt) {
	// 					_bAltHeld = event.IsPressed();
	// 				}

	// 				io.AddKeyEvent(ImGuiKey_ModShift, _bShiftHeld);
	// 				io.AddKeyEvent(ImGuiKey_ModCtrl, _bCtrlHeld);
	// 				io.AddKeyEvent(ImGuiKey_ModAlt, _bAltHeld);

	// 				if (!bShowMain) {
	// 					if (event.keyCode == Settings::uToggleUIKeyData[0] && event.IsDown()) {
	// 						if (_bCtrlHeld == static_cast<bool>(Settings::uToggleUIKeyData[1]) && _bShiftHeld == static_cast<bool>(Settings::uToggleUIKeyData[2]) && _bAltHeld == static_cast<bool>(Settings::uToggleUIKeyData[3])) {
	// 							bShowMain = true;
	// 						}
	// 					}
	// 				} else {
	// 					switch (event.device) {
	// 					case RE::INPUT_DEVICE::kMouse:
	// 						if (event.keyCode > 7) {
	// 							io.AddMouseWheelEvent(0, event.value * (event.keyCode == 8 ? 1 : -1));
	// 						} else {
	// 							if (event.keyCode > 5)
	// 								event.keyCode = 5;
	// 							io.AddMouseButtonEvent(event.keyCode, event.IsPressed());
	// 						}
	// 						break;
	// 					case RE::INPUT_DEVICE::kKeyboard:
	// 						if (event.keyCode == 0x1 && event.IsDown()) {
	// 							// 0x1 = escape
	// 							bShowMain = false;
	// 						} else if (event.keyCode == Settings::uToggleUIKeyData[0] && event.IsDown() && io.KeyCtrl == static_cast<bool>(Settings::uToggleUIKeyData[1]) && io.KeyShift == static_cast<bool>(Settings::uToggleUIKeyData[2]) && io.KeyAlt == static_cast<bool>(Settings::uToggleUIKeyData[3])) {
	// 							bShowMain = false;
	// 						} else {
	// 							io.AddKeyEvent(imGuiKey, event.IsPressed());

	// 							if (key < 0xA0 || key > 0xA5) {
	// 								// exclude modifier keys
	// 								_lastKeyPressed = event.keyCode;
	// 							}
	// 						}
	// 						break;
	// 					}
	// 					break;
	// 				}
	// 			}
	// 		}
	// 	}

	// 	_keyEventQueue.clear();
	// }
}