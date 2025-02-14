#include "Hooks/Skyrim/Input.hpp"
#include "Managers/InputManager.hpp"

using namespace RE;
using namespace GTS;

namespace Hooks {

	void Hook_Input::DispatchEvent(RE::BSTEventSource<RE::InputEvent*>* a_dispatcher, RE::InputEvent** a_events) {
		if (!a_events) {
			_DispatchEvent(a_dispatcher, a_events);
			return;
		}

		InputManager::GetSingleton().ProcessEvents(a_events);
		_DispatchEvent(a_dispatcher, a_events);

	}

	void Hook_Input::Hook(Trampoline& trampoline) {
		const REL::Relocation<uintptr_t> inputHook{ REL::VariantID(67315, 68617, 0xC519E0) };
		_DispatchEvent = trampoline.write_call<5>(inputHook.address() + REL::VariantOffset(0x7B, 0x7B, 0x81).offset(), DispatchEvent);
		logger::info("Hooked Input::DispatchEvent");
	}

}
