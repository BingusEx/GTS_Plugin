#include "UI/ImGui/ImWindowManager.hpp"
#include "UI/ImGui/ImUtil.hpp"

namespace GTS {

    void ImWindowManager::AddWindow(std::unique_ptr<ImWindow> a_window) {

        assert(a_window != nullptr);

        if (HasWindows()) {
            for (const auto& window : windows) {
                if (window->Name == a_window->Name) {
                    logger::warn("ImWindowManager::AddWindow: Window with name {} already exists, Not Adding New Window", a_window->Name);
                    return;
                }
            }
        }

        windows.push_back(std::move(a_window));
        logger::info("ImWindowManager::AddWindow {}", windows.back()->Name);
    }

    void ImWindowManager::Update() {

		if (HasWindows()) [[likely]] {

            if (HasInputConsumers()) {
                auto& io = ImGui::GetIO();
                io.MouseDrawCursor = true;
            }
            else {
                auto& io = ImGui::GetIO();
                io.MouseDrawCursor = false;
                io.ClearInputKeys();
                io.ClearEventsQueue();
                io.ClearInputMouse();
            }

            if (ShowMetrics) {
                ImGui::ShowMetricsWindow();
            }

            else if (ShowStack) {
                ImGui::ShowStackToolWindow();
            }

            if (Plugin::AnyMenuOpen()) {
                return;
            }

            for (const auto& window : windows) {


                if (window->ShouldShow()) {

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, window->GetAlpha());
                    ImGui::PushFont(ImFontManager::GetFont("text")); //Default font

                    volatile bool PushedVars = false;

                    if (!window->DrawBG) {
                        ImGui::SetNextWindowBgAlpha(0.f);
                    }

                    ImGui::Begin(window->Name.c_str(), &window->Show, window->flags);
                    window->Draw();

                    if (PushedVars) {
                        ImGui::PopStyleColor();
                    }

                    ImGui::End();
                    ImGui::PopStyleVar();
                    ImGui::PopFont();
                }
            }
        }
    }

    //Gets a ptr to the window which fully matches the "Name" var.
    //Name var gets set in the ctor of the child window, otherwise its "Default"
    //If 2 Or more default windows exist only the earliest one will be returned
    ImWindow* ImWindowManager::GetWindowByName(const std::string& a_name) const {
        for (const auto& window : windows) {
            if (window->Name == a_name) {
                return window.get();
            }
        }
        logger::error("ImWindowManager::GetWindowByName Name: {} does not exist, i hope you have nullptr checks...", a_name);
        return nullptr;
    }
}
