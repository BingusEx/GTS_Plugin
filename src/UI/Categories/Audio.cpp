#include "Audio.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"

using namespace GtsUI;

void CategoryAudio::DrawLeft(){

    ImUtil_Unique {

        const char* T0 = "Enable footstep sounds when player size meets or exceeds a certain threshold.";
        const char* T1 = "Enable moaning sounds during spells like Slow Growth.";
        
        if(ImGui::CollapsingHeader("Sounds",ImUtil::HeaderFlags)){ 
            ImUtil::CheckBox("Footstep Sounds",&Settings.bFootstepSounds,T0);
            ImUtil::CheckBox("Moans On Slow Growth",&Settings.bSlowGrowMoans, T1);
            ImGui::Spacing();
        }
    }

    ImUtil_Unique {

        const char* T0 = "Alter the voiceline pitch of NPCs when small/large.";
        const char* T1 = "Change the maximum voice pitch, Higher values will lower the pitch when the actor is large.\n"
                         "It's recommended to leave this at 1.0x. Anything above 1.2x doesn't sound good.";

        if(ImGui::CollapsingHeader("Voice",ImUtil::HeaderFlags)){
            ImUtil::CheckBox("Enable Voice Override",&Settings.bEnableVoiceOverride, T0);
            ImUtil::SliderF("Voice Pitch Max",&Settings.fMaxVoiceFrequency, 1.0f, 1.6f, T1, "%.2fx", !Settings.bEnableVoiceOverride);
            ImGui::Spacing();
        }
    }

}

void CategoryAudio::DrawRight(){

    const char* T0 = "Adjust the volume of mod-added voices.";
    const char* T1 = "Change the volume of GTS effects.";
    const char* T2 = "Modify the volume of footstep sounds.";
    const char* T3 = "Modify the volume of secondary footstep effects.";
    const char* T4 = "Set the volume for impact sounds from actions like stomps.";

    if(ImGui::CollapsingHeader("Volume Settings",ImUtil::HeaderFlags)){ 
        ImUtil::SliderF("Voices",&Settings.fVolumeVoice, 0.0f, 1.0f, T0,"%.2fx");
        ImUtil::SliderF("Effects",&Settings.fVolumeEffects, 0.0f, 1.0f, T1,"%.2fx");
        ImUtil::SliderF("Footsteps",&Settings.fVolumeFootstep, 0.0f, 1.0f, T2,"%.2fx");
        ImUtil::SliderF("Footstep Effects",&Settings.fVolumeFootstepEffects, 0.0f, 1.0f, T3,"%.2fx");
        ImUtil::SliderF("Impacts",&Settings.fVolumeImpacts, 0.0f, 1.0f, T4,"%.2fx");
        ImGui::Spacing();
    }
}