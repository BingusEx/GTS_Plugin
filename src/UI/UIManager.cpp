#include "UI/DearImGui/imgui.h"
#include "UI/DearImGui/imgui_impl_dx11.h"
#include "UI/DearImGui/imgui_impl_win32.h"
#include "UIManager.hpp"

#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/Windows/WindowSettings.hpp"
#include "UI/Windows/WindowStatus.hpp"


namespace GtsUI {

    void UIManager::Init() {
        if (Initialized) {
            return;
        }

        const auto renderManager = RE::BSRenderManager::GetSingleton();

        const auto device = reinterpret_cast<ID3D11Device*>(renderManager->GetRuntimeData().forwarder);
        const auto context = reinterpret_cast<ID3D11DeviceContext*>(renderManager->GetRuntimeData().context);
        const auto swapChain = reinterpret_cast<IDXGISwapChain*>(renderManager->GetRuntimeData().swapChain);

        DXGI_SWAP_CHAIN_DESC sd{};
        swapChain->GetDesc(&sd);

        
        // RECT rect{};
        // if (GetClientRect(sd.OutputWindow, &rect) == TRUE) {
        // 	_userData.screenScaleRatio = { static_cast<float>(sd.BufferDesc.Width) / static_cast<float>(rect.right), static_cast<float>(sd.BufferDesc.Height) / static_cast<float>(rect.bottom) };
        // }
        // else {
        // 	_userData.screenScaleRatio = { 1.0f, 1.0f };
        // }

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        io.DisplaySize = { static_cast<float>(sd.BufferDesc.Width), static_cast<float>(sd.BufferDesc.Height) };
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        //io.UserData = &_userData;
        //io.IniFilename = Settings::imguiIni.data();
        io.IniFilename = "GtsPluginUI.ini";
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

        ImGui_ImplWin32_Init(sd.OutputWindow);
        ImGui_ImplDX11_Init(device, context);

        ImWindowManager& WinMgr = ImWindowManager::GetSingleton();

        WinMgr.Init();
        WinMgr.AddWindow(std::make_unique<WindowSettings>());
        WinMgr.AddWindow(std::make_unique<WindowStatus>());


        Initialized = true;

        logger::info("Imgui Init OK");
    }

    void UIManager::Render() {

        //TODO
        //Itterate through list of windows to render and render them


        if (!Initialized) {
            return;
        }


        ImGui_ImplWin32_NewFrame();
        ImGui_ImplDX11_NewFrame();
        ImGui::NewFrame();
        ImWindowManager::GetSingleton().Draw();


        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        //ProcessInputEventQueue();

        //bool bShouldDraw = false;
        //for (const auto& window : _windows) {
        //	if (window->ShouldDraw()) {
        //		bShouldDraw = true;
        //	}
        

        //if (!bShouldDraw) {
        //	// early out
        //	return;
        //}





        //for (const auto& window : _windows) {
        //	window->TryDraw();
        //}
    }

}


