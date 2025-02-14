#include "Hooks/Skyrim/Renderer.hpp"
#include "UI/UIManager.hpp"

using namespace RE;
using namespace GTS;
using namespace GTSUI;

namespace Hooks {

	//Win32 Window Messages
	LRESULT Hook_Renderer::WndProcHandler(HWND a_hwnd, UINT a_msg, WPARAM a_wParam, LPARAM a_lParam) {
		if (a_msg == WM_KILLFOCUS) {
			

		}
		return _WndProcHandler(a_hwnd, a_msg, a_wParam, a_lParam);
	}

	//D3D11 Swapchain
	void Hook_Renderer::CreateD3DAndSwapChain() {
		_CreateD3DAndSwapChain();
		UIManager::GetSingleton().Init();
	}

	//Register Window Class
	WORD Hook_Renderer::RegisterClassA(WNDCLASSA* a_wndClass) {
		_WndProcHandler = reinterpret_cast<uintptr_t>(a_wndClass->lpfnWndProc);
		a_wndClass->lpfnWndProc = &WndProcHandler;

		return _RegisterClassA(a_wndClass);
	}

	//DXGISwapchain::Present
	void Hook_Renderer::Present(uint32_t a1) {
		_Present(a1);
		UIManager::GetSingleton().Render();
	}

	void Hook_Renderer::Hook(Trampoline& trampoline) {
		//Credits to ershin for the hooks here
		//const REL::Relocation<uintptr_t> inputHook{ REL::VariantID(67315, 68617, 0xC519E0) };           // C150B0, C3B360, C519E0
		const REL::Relocation<uintptr_t> registerWindowHook{ REL::VariantID(75591, 77226, 0xDC4B90) };  // D71F00, DA3850, DC4B90
		const REL::Relocation<uintptr_t> created3d11Hook{ REL::VariantID(75595, 77226, 0xDC5530) };     // D72810, DA3850, DC5530
		const REL::Relocation<uintptr_t> presentHook{ REL::VariantID(75461, 77246, 0xDBBDD0) };         // D6A2B0, DA5BE0, DBBDD0


		//_InputFunc = trampoline.write_call<5>(inputHook.address() + REL::VariantOffset(0x7B, 0x7B, 0x81).offset(), InputFunc);

		_RegisterClassA = *(uintptr_t*)trampoline.write_call<6>(registerWindowHook.address() + REL::VariantOffset(0x8E, 0x15C, 0x99).offset(), RegisterClassA);
		logger::info("Hooked RegisterClassA");

		_CreateD3DAndSwapChain = trampoline.write_call<5>(created3d11Hook.address() + REL::VariantOffset(0x9, 0x275, 0x9).offset(), CreateD3DAndSwapChain);
		logger::info("Hooked SwapChain");

		_Present = trampoline.write_call<5>(presentHook.address() + REL::VariantOffset(0x9, 0x9, 0x15).offset(), Present);
		logger::info("Hooked Present");
	}

}
