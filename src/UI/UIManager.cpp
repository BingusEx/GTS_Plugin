#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_dx11.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UI/UIManager.hpp"
#include "Managers/Console/ConsoleManager.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"
#include "Managers/Input/InputManager.hpp"

namespace {

    void OpenSettingsImpl(bool a_IsConsole) {

        if (!GTS::Plugin::Ready()) {

            if (a_IsConsole) {
                GTS::Cprint("Can not show the settings menu at this time.");
            }

            return;
        }

        if (GTS::Plugin::AnyMenuOpen() || (RE::UI::GetSingleton()->IsMenuOpen(RE::Console::MENU_NAME) && !a_IsConsole)) {

            if (a_IsConsole) {
                GTS::Cprint("Can not show the settings menu at this time.");
            }

            return;
        }

        if (GTS::Plugin::AnyMenuOpen()) {

            if (a_IsConsole) {
                GTS::Cprint("Can not show the settings menu at this time.");
            }

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

            if (a_IsConsole) {
                const auto msgQueue = RE::UIMessageQueue::GetSingleton();
                msgQueue->AddMessage(RE::Console::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, nullptr);
            }
        }
    }

    void OpenSettingsK(const GTS::ManagedInputEvent& data) {
        OpenSettingsImpl(false);
    }

    void OpenSettingsC() {
        OpenSettingsImpl(true);
    }

}

namespace GTS {

    void UIManager::CloseSettings() {
        if (auto Window = GTS::ImWindowManager::GetSingleton().GetWindowByName("Settings")) {
            //Show Settings Window
            Window->Show = false;
            RE::Main::GetSingleton()->freezeTime = false;
        }
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

        InputManager::RegisterInputEvent("OpenSettings", OpenSettingsK);
        ConsoleManager::RegisterCommand("menu", OpenSettingsC, "Open the settings menu");
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


