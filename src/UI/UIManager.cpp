#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_dx11.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UIManager.hpp"

#include "Data/Plugin.hpp"

#include "ImGui/ImUtil.hpp"

#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"

#include "Managers/Input/InputManager.hpp"




namespace {

    void OpenSettings(const GTS::ManagedInputEvent& data) {

        if (!GTS::Plugin::Ready()) {
            return;
        }

        if (auto Window = GTS::ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
            Window->Show = true;
        }

        //Attempt to fix alt tab getting stuck
		//Doesnt work :( Can microsoft stop "fixing" api's that aren't broken smh
        //GTS::UIManager::GetSingleton().ReleaseStuckKeys();
        //GTS::UIManager::GetSingleton().OnFocusLost();

        //auto& io = ImGui::GetIO();
        //io.ClearInputKeys();
        //io.ClearEventsQueue();
        //io.ClearInputMouse();

    }

}

namespace GTS {

    void UIManager::Init() {

        if (Initialized.load()) {
            return;
        }

        const auto RenderManager = RE::BSRenderManager::GetSingleton();

        const auto D3DDevice = reinterpret_cast<ID3D11Device*>(RenderManager->GetRuntimeData().forwarder);
        const auto D3DContext = reinterpret_cast<ID3D11DeviceContext*>(RenderManager->GetRuntimeData().context);
        const auto DXGISwapChain = reinterpret_cast<IDXGISwapChain*>(RenderManager->GetRuntimeData().swapChain);

        DXGI_SWAP_CHAIN_DESC SwapChainDesc{};

		HRESULT hr = DXGISwapChain->GetDesc(&SwapChainDesc);

		if (FAILED(hr)) {
		    logger::error("Could not get the swapchain, HRESULT: {}", hr);
            ReportAndExit("Could not get the swapchain, HRESULT: {}");
		}

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.UserData = &ImGuiUserData;
        io.IniFilename = ImGuiINI.data();
        io.DisplaySize = { static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height) };
        io.ConfigNavEscapeClearFocusWindow = false;

        //Appears to do the exact opposite of what we want...
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        
        //Keyboard nav is too buggy if using borderless mode...
        //This tries to remmedy that slightly
        io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

        RECT rect{};
        if (GetClientRect(SwapChainDesc.OutputWindow, &rect) == TRUE) {
            ImGuiUserData.screenScaleRatio = {
            	.x = static_cast<float>(SwapChainDesc.BufferDesc.Width) / static_cast<float>(rect.right),
            	.y = static_cast<float>(SwapChainDesc.BufferDesc.Height) / static_cast<float>(rect.bottom)
            };
        }
        else {
            ImGuiUserData.screenScaleRatio = {
            	.x= 1.0f,
            	.y= 1.0f
            };
        }

        ImGui_ImplWin32_Init(SwapChainDesc.OutputWindow);
        ImGui_ImplDX11_Init(D3DDevice, D3DContext);

        FontMgr.Init();
    	StyleMgr.LoadStyle();

        WinMgr.AddWindow(std::make_unique<WindowSettings>());
        WinMgr.AddWindow(std::make_unique<WindowStatus>());

        Initialized.store(true);

        logger::info("ImGui Init OK");

        InputManager::RegisterInputEvent("OpenSettings", OpenSettings);

    }

    void UIManager::Render() {
        std::ignore = Profilers::Profile("UIManager::Render");

        if (!Initialized) {
            return;
        }

        ProcessInputEventQueue();

        if (!Plugin::Ready()) {
            return;
        }

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();

        ImGui::NewFrame();

        ImWindowManager::GetSingleton().Update();

        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    }
}


