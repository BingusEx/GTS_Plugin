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

        const auto& ActorTransient = Transient::GetSingleton().GetData(a_Actor);
        if (!ActorTransient) {
            ImGui::Text("Actor Invalid!");
            return;
        }
       
        const auto& ActorPersistent = Persistent::GetSingleton().GetData(a_Actor);
        if (!ActorPersistent) {
            ImGui::Text("Actor Invalid!");
            return;
        }

        const auto& AttributeManager = AttributeManager::GetSingleton();
        const auto& Settings = Config::GetUI().StatusWindow;

        float CarryWeight;
        //When in god mode carry weight gets 100x'ed for some reason
        if (a_Actor->formID == 0x14 && IsInGodMode(a_Actor)) {
            CarryWeight = ActorTransient->CarryWeightBoost / 100u;
        }
        else [[likely]] {
            CarryWeight = ActorTransient->CarryWeightBoost;
        }

        const float BonusSize = ActorTransient->PotionMaxSize;
        const float StolenHealth = ActorPersistent->stolen_health;
		const float StolenMagicka = ActorPersistent->stolen_magick;
		const float StolenStamina = ActorPersistent->stolen_stamin;
		const float StolenAttributes = ActorPersistent->stolen_attributes;
		const float SizeReserve = ActorPersistent->SizeReserve;
        const float SizeEssense = Persistent::GetSingleton().GTSExtraPotionSize.value;
        const float CurrentScale = get_visual_scale(a_Actor);
        const float MaxScale = get_max_scale(a_Actor);
        const float AspectOfGTS = Ench_Aspect_GetPower(a_Actor) * 100.0f;
        const float DamageResist = (1.0f - AttributeManager.GetAttributeBonus(a_Actor, ActorValue::kHealth)) * 100.f;
        const float Speed = (AttributeManager.GetAttributeBonus(a_Actor, ActorValue::kSpeedMult) - 1.0f) * 100.f;
        const float JumpHeight = (AttributeManager.GetAttributeBonus(a_Actor, ActorValue::kJumpingBonus) - 1.0f) * 100.0f;
        const float Damage = (AttributeManager.GetAttributeBonus(a_Actor, ActorValue::kAttackDamageMult) - 1.0f) * 100.0f;
        const float ShrinkResistance = (1.0f - Potion_GetShrinkResistance(a_Actor)) * 100.f;
        const float OnTheEdge = (GetPerkBonus_OnTheEdge(a_Actor,0.01f) - 1.0f) * 100.f;
        const float BonusHHDamage = GetHighHeelsBonusDamage(a_Actor, true) * 100.0f;


        const std::string StringScale = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitScale) ? fmt::format(" ({:.2f}x)", CurrentScale) : "";
        const std::string StringReal = hasFlag(a_featureFlags, GTSInfoFeatures::kUnitReal) ? GTS::GetFormatedHeight(a_Actor).c_str() : "";
        const std::string ResultingText = fmt::format("{}{}", StringReal, StringScale);

        const auto VisualProgress = MaxScale < 250.0f ? CurrentScale / MaxScale : 0.0f;

        //Visual Scale (Progress) Bar
        ImUtil::CenteredProgress(VisualProgress, ImVec2(hasFlag(a_featureFlags, GTSInfoFeatures::kAutoSize) ? 
            0.0f : 
            Settings.fFixedWidth, 0.0f), ResultingText.c_str(), Settings.fSizeBarHeightMult
        );

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowMaxSize)) {
            
        	if (MaxScale > 250.0f) {
                ImGui::Text("Max Scale: Infinite");
        	}
            else {
                ImGui::Text("Max Scale: %.2fx", MaxScale);
            }
        }

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowBonusSize))
            ImGui::Text("Bonus Size: %.2fx", BonusSize);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowHighHeelBonusDmg) && BonusHHDamage > 0.0f)
            ImGui::Text("High Heel Damage: +%.0f%%", BonusHHDamage);

        //Only Do anything on the player
        if (a_Actor->formID == 0x14) {

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowEssence))
                ImGui::Text("Essence: +%.2fx", SizeEssense);

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowShrinkResist))
                ImGui::Text("Shrink Resist: %.1f%%", ShrinkResistance);

            if (Runtime::HasPerk(a_Actor, "OnTheEdge")) {
                if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowOnTheEdge))
                    ImGui::Text("On The Edge: %.1f%%", OnTheEdge);
            }

            if (Runtime::HasPerk(a_Actor, "SizeReserve")) {
                if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowSizeReserve))
                    ImGui::Text("Size Reserve: %.2fx", SizeReserve);
            }
        }

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowWeight))
        	ImGui::Text("Weight: %s", GTS::GetFormatedWeight(a_Actor).c_str());

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowAspect))
        	ImGui::Text("Aspect of GTS: %.0f%%", AspectOfGTS);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgResist))
        	ImGui::Text("Bonus Damage Resist: %.1f%%", DamageResist);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowCarryWeight))
        	ImGui::Text("Bonus Carry Weight: %.1f", CarryWeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowSpeedmult))
        	ImGui::Text("Bonus Speed: %.1f%%", Speed);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowJumpMult))
        	ImGui::Text("Bonus Jump Height: %.1f%%", JumpHeight);

        if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowDmgMult))
        	ImGui::Text("Bonus Damage: %.1f%%", Damage);

        if (Runtime::HasPerk(a_Actor, "SizeConversion")) {
            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowStolenAttributes))
                ImGui::Text("Stored Attributes: +%.2f", StolenAttributes);
        }

        if (Runtime::HasPerk(a_Actor, "SoulVorePerk")) {

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowStolenHealth))
                ImGui::Text("Absorbed Health: +%.2f", StolenHealth);

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowStolenMagicka))
                ImGui::Text("Absorbed Magicka: +%.2f", StolenMagicka);

            if (hasFlag(a_featureFlags, GTSInfoFeatures::kShowStolenStamina))
                ImGui::Text("Absorbed Stamina: +%.2f", StolenStamina);
        }
    }
}
