#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_dx11.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UI/UIManager.hpp"

#include "UI/ImGui/ImUtil.hpp"

#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"

#include "Managers/Input/InputManager.hpp"

namespace {

    void OpenSettings(const GTS::ManagedInputEvent& data) {

        if (!GTS::Plugin::Ready()) {
            return;
        }

        if (GTS::Plugin::AnyMenuOpen()) {
            RE::DebugNotification("A menu is open, please close it before opening GTS Settings.");
            return;
        }

        if (RE::UI::GetSingleton()->IsMenuOpen(RE::DialogueMenu::MENU_NAME)) {
            RE::DebugNotification("The dialogue menu is open, please close it first.");
            return;
        }

        if (auto Window = GTS::ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
            //Show Settings Window
            Window->Show = true;

            //Freeze the game
            RE::Main::GetSingleton()->freezeTime = true;
        }
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
        io.IniFilename = ImGuiINI.data();
        io.DisplaySize = { static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height) };
        io.ConfigNavEscapeClearFocusWindow = false;
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        
        //Keyboard nav is too buggy if using borderless mode...
        //This tries to remmedy that slightly
        io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;

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


    //Gets called last so it will overlay everything
    void UIManager::Update() {
        std::ignore = Profilers::Profile("UIManager::Update");

        if (!Initialized.load()) {
            return;
        }

        if (!Plugin::Ready()) {
            return;
        }

        ProcessInputEventQueue();

        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();

        ImGui::NewFrame();

        ImWindowManager::GetSingleton().Update();

        ImGui::Render();

        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ImFontManager::GetSingleton().ProcessActions();

    }
}


