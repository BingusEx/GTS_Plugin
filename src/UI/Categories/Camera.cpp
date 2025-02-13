#include "Camera.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

using namespace GtsUI;

inline void DrawCameraOffsets(const char* a_title, const char* a_toolip, std::array<float, 3>* a_offsets) {
    ImGui::Spacing();

    ImGui::Text(a_title);
    ImGui::BeginGroup();

    const char* T0 = "Move the camera left or right.";
    const char* T1 = "Move the camera up or down.";
    const char* T2 = "Move the camera forward or backward.";

    ImUtil::SliderF("Left - Right", &a_offsets->at(0), -100.f, 100.f, T0, "%.1f");
    ImUtil::SliderF("Up - Down", &a_offsets->at(2), -200.f, 200.f, T1, "%.1f");
    ImUtil::SliderF("Forward - Back", &a_offsets->at(1), -200.f, 200.f, T2, "%.1f");

    ImGui::EndGroup();

    // Applies to the whole group
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(a_toolip);
    }
}

void DrawCameraSettings(CameraOffsets* a_set, const char* a_title) {
    const char* T0 = "Select which biped skeleton bone the camera should track.";

    if (ImGui::CollapsingHeader(a_title, ImGuiTreeNodeFlags_None)) {
        ImUtil::ComboEx<CameraTrackingUsr>("Center On Bone", a_set->sCenterOnBone, T0);

        ImUtil_Unique {
            DrawCameraOffsets(
                "Offsets | Standing",
                "Adjust camera offsets when standing.",
                &a_set->f3NormalStand
            );
        }

        ImUtil_Unique {
            DrawCameraOffsets(
                "Offsets | Standing Combat",
                "Adjust camera offsets when standing and in combat.",
                &a_set->f3CombatStand
            );
        }

        ImUtil_Unique {
            DrawCameraOffsets(
                "Offsets | Sneaking",
                "Adjust camera offsets while sneaking, crawling, or prone.",
                &a_set->f3NormalSneak
            );
        }

        ImUtil_Unique {
            DrawCameraOffsets(
                "Offsets | Sneaking Combat",
                "Adjust camera offsets while sneaking, crawling, or prone and in combat.",
                &a_set->f3CombatSneak
            );
        }
        ImGui::Spacing();
        ImGui::Spacing();
    }
}

void CategoryCamera::DrawLeft() {
    ImUtil_Unique {

        const char* T0 = "Change the intensity of camera shakes when performing actions as a player.";
        const char* T1 = "Change the intensity of camera shakes for NPCs.";

        if (ImGui::CollapsingHeader("Camera Shake", ImUtil::HeaderFlags)) {
            ImUtil::SliderF("Player Total Shake Power", &Settings.fCameraShakePlayer, 0.1f, 3.0f, T0, "%.1fx");
            ImUtil::SliderF("NPC Total Shake Power", &Settings.fCameraShakeOther, 0.1f, 3.0f, T1, "%.1fx");

            ImGui::Spacing();
        }
    }

    ImUtil_Unique {

        const char* T0 = "Change the height multiplier of the camera while crawling in first person.";
        const char* T1 = "Change the height multiplier of the camera while crawling in third person.";

        if (ImGui::CollapsingHeader("Crawl Height", ImUtil::HeaderFlags)) {
            ImUtil::SliderF("1st P. Crawl Height", &Settings.fFPCrawlHeightMult, 0.01f, 1.0f, T0, "%.1fx");
            ImUtil::SliderF("3rd P. Crawl Height", &Settings.fTPCrawlHeightMult, 0.01f, 1.0f, T1, "%.1fx");

            ImGui::Spacing();
        }
    }

    ImUtil_Unique {

        const char* T0 = "Enable camera collision with actors.";
        const char* T1 = "Enable camera collision with trees.";
        const char* T2 = "Enable camera collision with debris (physics-enabled objects).";
        const char* T3 = "Enable camera collision with terrain.";
        const char* T4 = "Enable camera collision with statics (basically any solid, non-movable object).";
        const char* T5 = "Change the scale at which the above collision settings should apply.";

        if (ImGui::CollapsingHeader("Camera Collision", ImGuiTreeNodeFlags_None)) {
            ImUtil::CheckBox("Collide With Actors", &Settings.bCamCollideActor, T0);
            ImGui::SameLine();
            ImUtil::CheckBox("Collide With Trees", &Settings.bCamCollideTree, T1);
            ImUtil::CheckBox("Collide With Debris", &Settings.bCamCollideDebris, T2);
            ImGui::SameLine();
            ImUtil::CheckBox("Collide With Terrain", &Settings.bCamCollideTerrain, T3);
            ImUtil::CheckBox("Collide With Statics", &Settings.bCamCollideStatics, T4);

            ImUtil::SliderF("Apply at Scale", &Settings.fModifyCamCollideAt, 1.0f, 50.0f, T5, "%.1fx");

            ImGui::Spacing();
        }
    }

    ImUtil_Unique {

        const char* T0 = "Change the minimum zoom distance.";
        const char* T1 = "Change the maximum zoom distance.";
        const char* T2 = "Change the zoom speed.";
        const char* T3 = "Change the zoom step.";

        const char* THelp = 
            "These are the same settings as can be found in skyrim.ini.\n"
            "They're added here for convenience.\n"
            "Note: These values are modified through hooks,\n"
            "so no matter what values you set in any ini file, they will be overridden by the offsets set here.";

        if (ImGui::CollapsingHeader("Skyrim Camera Settings", ImGuiTreeNodeFlags_None)) {
            ImGui::TextColored(ImUtil::ColorSubscript, "What is this (?)");

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip(THelp);
            }

            ImUtil::SliderF("Minimum Distance", &Settings.fCameraDistMin, -10.0f, 100.0f, T0, "%.1f");
            ImUtil::SliderF("Maximum Distance", &Settings.fCameraDistMax, 200.0f, 700.0f, T1, "%.1f");
            ImUtil::SliderF("Zoom Speed", &Settings.fCameraZoom, 5.0f, 40.0f, T2, "%.1f");
            ImUtil::SliderF("Zoom Step", &Settings.fCameraStep, 0.0f, 0.3f, T3, "%.3f");

            ImGui::Spacing();
        }
    }

}

void CategoryCamera::DrawRight() {
    ImUtil_Unique {
        const char* T0 = "Enable automatic camera.";
        const char* T1 = "Change the third-person camera mode.";
        const char* T2 = "Change the first-person camera mode.";

        if (ImGui::CollapsingHeader("Automatic Camera", ImUtil::HeaderFlags)) {
            ImUtil::CheckBox("Enable Automatic Camera", &Settings.bAutomaticCamera, T0);
            ImUtil::ComboEx<CameraModeTP>("TP Camera Mode", Settings.sAutoCameraModeTP, T1, !Settings.bAutomaticCamera);
            ImUtil::ComboEx<CameraModeFP>("FP Camera Mode", Settings.sAutoCameraModeFP, T2, !Settings.bAutomaticCamera);

            ImGui::Spacing();
        }
    }

    ImGui::BeginDisabled(!Settings.bAutomaticCamera);

    ImUtil_Unique {
        DrawCameraSettings(&Settings.OffsetsNormal, "Normal Camera");
    }

    ImUtil_Unique {
        DrawCameraSettings(&Settings.OffsetsAlt, "Alternative Camera");
    }

    ImUtil_Unique {
        const char* T0 = "Offset the foot cameras forward or backward.";

        if (ImGui::CollapsingHeader("Foot Camera", ImGuiTreeNodeFlags_None)) {
            ImUtil::SliderF("Forward - Back", &Settings.fFootCameraFBOffset, -200.f, 200.f, T0, "%.1f");

            ImGui::Spacing();
        }
    }

    ImGui::EndDisabled();

}