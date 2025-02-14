#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_dx11.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UIManager.hpp"

#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"

#include "Managers/InputManager.hpp"


namespace GTS {

    void OpenSettings(const ManagedInputEvent& data) {
        ImWindowManager::GetSingleton().GetWindowByName("Settings")->Show = true;
    }

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
		    logger::error("Could Not Get Swapchain, HRESULT: {}", hr);
		    SKSE::stl::report_and_fail("Could Not Get Swapchain");
		}

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.UserData = &ImGuiUserData;
        io.IniFilename = ImGuiINI.data();
        io.DisplaySize = { static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height) };
        io.ConfigNavEscapeClearFocusWindow = false;

        //Appears to do the exact opposite of what we want...
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

        RECT rect{};
        if (GetClientRect(SwapChainDesc.OutputWindow, &rect) == TRUE) {
            ImGuiUserData.screenScaleRatio = { static_cast<float>(SwapChainDesc.BufferDesc.Width) / static_cast<float>(rect.right), static_cast<float>(SwapChainDesc.BufferDesc.Height) / static_cast<float>(rect.bottom) };
        }
        else {
            ImGuiUserData.screenScaleRatio = { 1.0f, 1.0f };
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

        if (!Initialized) {
            return;
        }

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();

        ImGui::NewFrame();

        ImWindowManager::GetSingleton().Update();

        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ProcessInputEventQueue();

    }



}


