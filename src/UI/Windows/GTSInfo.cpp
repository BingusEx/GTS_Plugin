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
        float MaxScale = get_max_scale(a_Actor);

        auto& Atrib = AttributeManager::GetSingleton();


        const float _aspectOfGTS = Ench_Aspect_GetPower(a_Actor) * 100.0f;
        const float _damageResist = (1.0f - Atrib.GetAttributeBonus(a_Actor, ActorValue::kHealth)) * 100.f;

        float _BonusSize = 0.0f;
        float _carryWeight = 0.0f;
        if (auto transient = Transient::GetSingleton().GetData(a_Actor)) {

            _BonusSize = transient->potion_max_size;

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
        const float _ShrinkResist = (1.0f - Potion_GetShrinkResistance(a_Actor)) * 100.f;
        const float _OnTheEdge = (GetPerkBonus_OnTheEdge(a_Actor,0.01f) - 1.0f) * 100.f;

        const float _Essence = Persistent::GetSingleton().GTSExtraPotionSize.value;
        const auto& Settings = Config::GetUI().StatusWindow;

        const std::string sScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format(" ({:.2f}x)", CurrentScale) : "";
        const std::string sReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetFormatedHeight(a_Actor).c_str() : "";
        const std::string pText = fmt::format("{}{}", sReal, sScale);

        const auto VisualProgress = MaxScale < 250.0f ? CurrentScale / MaxScale : 0.0f;

        ImUtil::CenteredProgress(VisualProgress, ImVec2(hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 0.0f : Settings.fFixedWidth, 0.0f), pText.c_str(), Settings.fSizeBarHeightMult);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowMaxSize)) {
            
        	if (MaxScale > 250.0f) {
                ImGui::Text("Max Scale: Infinite");
        	}
            else {
                ImGui::Text("Max Scale: %.2fx", MaxScale);
            }
        }


        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowBonusSize))
            ImGui::Text("Bonus Size: %.2fx", _BonusSize);

        if (a_Actor->formID == 0x14) {
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowEssence))
                ImGui::Text("Essence: +%.2fx", _Essence);

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowShrinkResist))
                ImGui::Text("Shrink Resist: %.1f%%", _ShrinkResist);

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowOnTheEdge))
                ImGui::Text("On The Edge: %.1f%%", _OnTheEdge);
        }

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowWeight))
        	ImGui::Text("Weight: %s", GTS::GetFormatedWeight(a_Actor).c_str());

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowAspect))
        	ImGui::Text("Aspect of GTS: %.0f%%", _aspectOfGTS);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgResist))
        	ImGui::Text("Bonus Dmg Resist: %.1f%%", _damageResist);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowCarryWeight))
        	ImGui::Text("Bonus Carry Weight: %.1f", _carryWeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowSpeedmult))
        	ImGui::Text("Bonus Speed: %.1f%%", _speed);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowJumpMult))
        	ImGui::Text("Bonus Jump Height: %.1f%%", _jumpHeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgMult))
        	ImGui::Text("Bonus Damage: %.1f%%", _damage);




    }
}
