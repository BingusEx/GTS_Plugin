#include "UI/Windows/GTSInfo.hpp"
#include "UI/ImGui/ImUtil.hpp"
#include "UI/DearImGui/imgui.h"

#include "Managers/Attributes.hpp"
#include "Utils/UnitConverter.hpp"

namespace GTS {

    void DrawGTSInfo(const GTSInfoFeatures a_featureFlags, RE::Actor* a_Actor) {

        if (!a_Actor) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        if (!a_Actor->Get3D()) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        if (!a_Actor->Is3DLoaded()) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        const float CurrentScale = get_visual_scale(a_Actor);
        const float MaxScale = get_max_scale(a_Actor);

        auto& Atrib = AttributeManager::GetSingleton();


        const float _aspectOfGTS = Ench_Aspect_GetPower(a_Actor) * 100.0f;
        const float _damageResist = (1.0f - Atrib.GetAttributeBonus(a_Actor, ActorValue::kHealth)) * 100.f;


        float _carryWeight = 0.0f;
        if (auto transient = Transient::GetSingleton().GetData(a_Actor)) {

            //When in god mode carry weight gets 100x'ed for some reason
            if (a_Actor->formID == 0x14 && IsInGodMode(a_Actor)) {
                _carryWeight = transient->carryweight_boost / 100u;
            }
            else [[likely]] {
                _carryWeight = transient->carryweight_boost;
                }
        }

        const float _speed = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kSpeedMult) - 1.0f) * 100.f;
        const float _jumpHeight = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kJumpingBonus) - 1.0f) * 100.0f;
        const float _damage = (Atrib.GetAttributeBonus(a_Actor, ActorValue::kAttackDamageMult) - 1.0f) * 100.0f;


        //TODO NEEDS TO BE CHANGED
        const float _PotionSize = Runtime::GetGlobal("ExtraPotionSize")->value;

        const std::string sScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format(" ({:.2f}x)", CurrentScale) : "";
        const std::string sReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetFormatedHeight(a_Actor).c_str() : "";
        const std::string pText = fmt::format("{}{}", sReal, sScale);
        ImUtil::CenteredProgress(CurrentScale / MaxScale, ImVec2(hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : 160.0f, 0.0f), pText.c_str());

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowMaxSize))
				ImGui::Text("Max Scale: %.2fx", MaxScale);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowWeight))
				ImGui::Text("Weight: %s", GTS::GetFormatedWeight(a_Actor).c_str());

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowAspect))
				ImGui::Text("Aspect of GTS: %.0f%%", _aspectOfGTS);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgResist))
				ImGui::Text("Bonus Dmg Resist: %.1f%%", _damageResist);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowCarryW))
				ImGui::Text("Bonus Carry Weight: %.1f", _carryWeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowSpeedmult))
				ImGui::Text("Bonus Speed: %.1f%%", _speed);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowJumpMult))
				ImGui::Text("Bonus Jump Height: %.1f%%", _jumpHeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgMult))
				ImGui::Text("Bonus Damage: %.1f%%", _damage);

        if (a_Actor->formID == 0x14) {
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowPermanentSize))
            	ImGui::Text("Essense: +%.2fx", _PotionSize);
        }
    }
}