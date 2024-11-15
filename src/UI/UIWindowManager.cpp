#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "UIWindowManager.hpp"
#include "UIWindow.hpp"


using namespace RE;
using namespace SKSE;

namespace Gts {

	bool UIWindowManager::HasWindows() {
		return imWindows.size() > 0;
	}

	void UIWindowManager::AddWindow(std::unique_ptr<UIWindow> a_window) {
		imWindows.push_back(std::move(a_window));
		logger::info("UIWindowManager::AddWindow {}", imWindows.back().get()->GetData()->Name);
	}

	void UIWindowManager::Update() {
		if (HasWindows()) {
			for (const auto& imWindow : imWindows) {
				if (imWindow->GetData()->Show) {
					logger::trace("UIWindowManager::Update()");
					imWindow->Update();
				}
			}
		}
	}
}
//
//void __stdcall UIWindowManager::RenderWidget() {
//    auto viewport = ImGui::GetMainViewport();
//    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });
//    ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.8f, viewport->Size.y * 0.8f }, ImGuiCond_Appearing);
//
//    // Animate a simple progress bar
//    static float progress = 0.0f, progress_dir = 1.0f;
//    progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
//    if (progress >= +1.1f) {
//        progress = +1.1f; progress_dir *= -1.0f;
//    }
//    if (progress <= -0.1f) {
//        progress = -0.1f; progress_dir *= -1.0f;
//    }
//
//    // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use all available width,
//    // or ImVec2(width,0.0f) for a specified width. ImVec2(0.0f,0.0f) uses ItemWidth.
//    ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
//    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
//    ImGui::Text("Progress Bar");
//
//    float progress_saturated = IM_CLAMP(progress, 0.0f, 1.0f);
//    char buf[32];
//    sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
//    ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
//
//    // Pass an animated negative value, e.g. -1.0f * (float)ImGui::GetTime() is the recommended value.
//    // Adjust the factor if you want to adjust the animation speed.
//    ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(0.0f, 0.0f), "Searching..");
//    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
//    ImGui::Text("Indeterminate");
//}
//
//
//void __stdcall UI::RenderMenuWindow() {
//
//
//
//
//    auto viewport = ImGui::GetMainViewport();
//    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_Appearing, ImVec2{ 0.5f, 0.5f });
//    ImGui::SetNextWindowSize(ImVec2{ viewport->Size.x * 0.8f, viewport->Size.y * 0.8f }, ImGuiCond_Appearing);
//    ImGuiWindowFlags window_flags = 0;
//    window_flags |= ImGuiWindowFlags_NoCollapse;
//    ImGui::Begin("Mod Control Panel", nullptr, window_flags);
//
//    float offset1 = 20.0f;
//    float offset2 = 20.0f;
//    float offset3 = 0.0f;
//
//    if (Config::MenuStyle != MenuStyle::Classic) {
//        offset1 = 50.0f;
//        offset2 = 41.0f;
//        offset3 = 5.0f;
//    }
//
//
//    ImGui::BeginChild("TreeView2", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, offset1), ImGuiChildFlags_None,
//        window_flags);
//    filter.Draw("##SKSEModControlPanelMenuFilter", -FLT_MIN);
//    ImGui::EndChild();
//    ImGui::SameLine();
//    ImGui::BeginChild("SKSEModControlPanelModMenuHeader", ImVec2(0, offset2), ImGuiChildFlags_None, window_flags);
//    if (display_node) {
//        auto windowWidth = ImGui::GetWindowSize().x;
//        auto textWidth = ImGui::CalcTextSize(display_node->Title.c_str()).x;
//        float offsetX = (windowWidth - textWidth) * 0.5f;
//        ImGui::SetCursorPosX(offsetX);
//        ImGui::SetCursorPosY(offset3);
//        ImGui::Text(display_node->Title.c_str());
//    }
//    else {
//        // TODO: Default title
//    }
//    ImGui::EndChild();
//
//    ImGui::BeginChild("SKSEModControlPanelTreeView", ImVec2(ImGui::GetContentRegionAvail().x * 0.3f, -FLT_MIN),
//        ImGuiChildFlags_Border, window_flags);
//    node_id = 0;
//    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 5.0f));
//    for (const auto& item : RootMenu->Children) {
//        if (filter.PassFilter(item.first.c_str()) &&
//            (ImGui::CollapsingHeader(std::format("{}##{}", item.first, node_id).c_str()))) {
//            for (auto node : item.second->SortedChildren) {
//                RenderNode(node);
//
//            }
//        }
//        else {
//            for (auto node : item.second->Children) {
//                DummyRenderer(node);
//            }
//        }
//    }
//    ImGui::PopStyleVar();
//    ImGui::EndChild();
//    ImGui::SameLine();
//    ImGui::BeginChild("SKSEModControlPanelMenuNode", ImVec2(0, -FLT_MIN), ImGuiChildFlags_Border, window_flags);
//    if (display_node) {
//        display_node->Render();
//    }
//    ImGui::EndChild();
//    ImGui::End();
//}

//void UIWindowManager::SetWindowDimensions(float a_offsetX /*= 0.f*/, float a_offsetY /*= 0.f*/, float a_sizeX /*= -1.f*/, float a_sizeY /*= -1.f*/, , ImVec2 a_sizeMin /*= ImVec2(0, 0)*/, ImVec2 a_sizeMax /*= ImVec2(0, 0)*/, ImGuiCond_ a_cond /*= ImGuiCond_FirstUseEver*/) {
//	auto& io = ImGui::GetIO();
//
//	_sizeData.sizeMin = {
//		std::fmin(300.f, io.DisplaySize.x - 40.f),
//		std::fmin(200.f, io.DisplaySize.y - 40.f)
//	};
//
//	if (a_sizeMin.x != 0) {
//		_sizeData.sizeMin.x = a_sizeMin.x;
//	}
//	if (a_sizeMin.y != 0) {
//		_sizeData.sizeMin.y = a_sizeMin.y;
//	}
//
//	_sizeData.sizeMax = {
//		io.DisplaySize.x,
//		std::fmax(io.DisplaySize.y - 40.f, _sizeData.sizeMin.y)
//	};
//
//	if (a_sizeMax.x != 0) {
//		_sizeData.sizeMax.x = a_sizeMax.x;
//	}
//	if (a_sizeMax.y != 0) {
//		_sizeData.sizeMax.y = a_sizeMax.y;
//	}
//
//	switch (a_alignment) {
//		case WindowAlignment::kTopLeft:
//			_sizeData.pivot = { 0.f, 0.f };
//			_sizeData.pos = {
//				std::fmin(20.f + a_offsetX, io.DisplaySize.x - 40.f),
//				20.f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kTopCenter:
//			_sizeData.pivot = { 0.5f, 0.f };
//			_sizeData.pos = {
//				io.DisplaySize.x * 0.5f + a_offsetX,
//				20.f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kTopRight:
//			_sizeData.pivot = { 1.f, 0.f };
//			_sizeData.pos = {
//				std::fmax(io.DisplaySize.x - 20.f - a_offsetX, 40.f),
//				20.f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kCenterLeft:
//			_sizeData.pivot = { 0.f, 0.5f };
//			_sizeData.pos = {
//				20.f + a_offsetX,
//				io.DisplaySize.y * 0.5f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kCenter:
//			_sizeData.pivot = { 0.5f, 0.5f };
//			_sizeData.pos = {
//				io.DisplaySize.x * 0.5f + a_offsetX,
//				io.DisplaySize.y * 0.5f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kCenterRight:
//			_sizeData.pivot = { 1.f, 0.5f };
//			_sizeData.pos = {
//				std::fmax(io.DisplaySize.x - 20.f - a_offsetX, 40.f),
//				io.DisplaySize.y * 0.5f + a_offsetY
//			};
//			break;
//		case WindowAlignment::kBottomLeft:
//			_sizeData.pivot = { 0.f, 1.f };
//			_sizeData.pos = {
//				std::fmin(20.f + a_offsetX, io.DisplaySize.x - 40.f),
//				io.DisplaySize.y - 20.f - a_offsetY
//			};
//			break;
//		case WindowAlignment::kBottomCenter:
//			_sizeData.pivot = { 0.5f, 1.f };
//			_sizeData.pos = {
//				io.DisplaySize.x * 0.5f + a_offsetX,
//				io.DisplaySize.y - 20.f - a_offsetY
//			};
//			break;
//		case WindowAlignment::kBottomRight:
//			_sizeData.pivot = { 1.f, 1.f };
//			_sizeData.pos = {
//				std::fmax(io.DisplaySize.x - 20.f - a_offsetX, 40.f),
//				io.DisplaySize.y - 20.f - a_offsetY
//			};
//			break;
//	}
//
//	_sizeData.size = {
//		a_sizeX < 0.f ? io.DisplaySize.x : a_sizeX,
//		a_sizeY < 0.f ? io.DisplaySize.y : a_sizeY
//	};
//
//	ImGui::SetNextWindowPos(_sizeData.pos, a_cond, _sizeData.pivot);
//	if (a_sizeX != 0.f || a_sizeY != 0.f) {
//		ImGui::SetNextWindowSize(_sizeData.size, a_cond);
//		ImGui::SetNextWindowSizeConstraints(_sizeData.sizeMin, _sizeData.sizeMax);
//	}
//}