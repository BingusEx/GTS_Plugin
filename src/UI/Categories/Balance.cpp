#include "UI/Categories/Balance.hpp"
#include "UI/DearImGui/imgui.h"
#include "UI/ImGui/ImUtil.hpp"



namespace {


    const std::string CollossalGrowthPerk = "ColossalGrowth"; //AKA GtsTotalSizeControl
    const std::string PleasurableGrowthPerk = "RandomGrowth"; //AKA GtsCrushGrowthAug
    const std::string CrushGrowthAugmetationPerk = "GrowthDesirePerk";
    const std::string GrowOnHitPerk = "GrowthOnHitPerk";

}

namespace GTS {



    void CategoryBalance::DrawLeft() {

        ImUtil_Unique{

            const char* T0 = "Enable/disable balance mode.";
            const char* T1 = "Penalty multiplier to all size gain if balance mode is enabled.";
            const char* T2 = "Shrink over time multiplier, works both in and out of combat. ";
            const char* T3 = "When in combat, multiplies the shrink modifier above.";
            const char* T4 = "Affects amount of size you lose when being hit.";

            const char* THelp = "Balance mode makes it harder to gain size in general.\n"
                                "Actors resist size damage while they have stamina and\n"
                                "you constantly shrink on hit and out of combat.\n"
                                "On top of that all attribute increases are 50%% weaker and some configuration options are disabled.\n"
                                "You cannot skip the quest while Balance Mode is enabled.";

            if (ImGui::CollapsingHeader("Balance Mode", ImUtil::HeaderFlags)) {

                ImGui::TextColored(ImUtil::ColorSubscript,"What is balance mode (?)");
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip(THelp);
                }

                ImUtil::CheckBox("Enable Balance Mode", &Settings.bBalanceMode, T0);

                ImGui::BeginDisabled(!Settings.bBalanceMode);

                ImUtil::SliderF("Size Gain Penalty", &Settings.fBMSizeGainPenaltyMult, 1.0f, 10.0f, T1, "%.2fx");
                ImUtil::SliderF("Shrink Rate", &Settings.fBMShrinkRate, 0.01f, 10.0f, T2, "%.2fx");
                ImUtil::SliderF("Shrink Rate Combat", &Settings.fBMShrinkRateCombat, 0.01f, 1.0f, T3, "%.2fx");
                ImUtil::SliderF("Shrink On Hit", &Settings.fBMShrinkOnHitMult, 0.01f, 2.0f, T4, "%.2fx");

                ImGui::EndDisabled();

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

            const char* T0 = "Toggle whether the player should receive size-related damage.";
            const char* T1 = "Toggle whether followers should receive size-related damage.";
            const char* T2 = "Toggle whether the player and followers stagger / ragdoll due to others' size.";
            const char* T3 = "Toggle whether other NPCs stagger / ragdoll due to others' size.";

            if (ImGui::CollapsingHeader("Misc Settings", ImUtil::HeaderFlags)) {

                ImUtil::CheckBox("Player Size Damage Immunity", &Settings.bPlayerFriendlyImmunity, T0);
                ImUtil::CheckBox("Followers Size Damage Immunity", &Settings.bFollowerFriendlyImmunity, T1);
                ImUtil::CheckBox("Allow Friendly Stagger", &Settings.bAllowFriendlyStagger, T2);
                ImUtil::CheckBox("Allow Stagger", &Settings.bAllowOthersStagger, T3);

                ImGui::Spacing();
            }
        }
    }

    void CategoryBalance::DrawRight() {

        ImUtil_Unique{

            const char* T0 = "Change the formula used for all size gain.";
            const char* T1 = "Adjust the global multiplier for all size gains and losses.";

            if (ImGui::CollapsingHeader("Size Options", ImUtil::HeaderFlags)) {

                if (ImUtil::ComboEx<SizeMode>("Size Gain Mode", Settings.sSizeMode, T0)) {
                    if (Settings.sSizeMode == "kNormal") Settings.fSpellEfficiency = 0.55f;
                    if (Settings.sSizeMode == "kMassBased") Settings.fSpellEfficiency = 0.33f;
                }

                ImUtil::SliderF("Efficiency Mult", &Settings.fSpellEfficiency, 0.1f, 1.0f, T1, "%.2fx");

                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

        	bool HasPerk = Runtime::HasPerk(PlayerCharacter::GetSingleton(), CollossalGrowthPerk);
			const std::string HeaderHelpText = Settings.bBalanceMode ? "Balance Mode Active" : "Requires \"Colossal Growth\" Perk";
            if (ImUtil::ConditionalHeader("Size Limits", HeaderHelpText, HasPerk && !Settings.bBalanceMode)) {
	            constexpr float Max = 255.0f;
	            constexpr float Min = 0.0;

                {   //Player Size
                    float* Scale = &Settings.fMaxPlayerSizeOverride;
                    const bool ShouldBeInf = *Scale > Max - 5.0f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1f;

                    std::string _Frmt;

                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {
                      
                        const float SkillBasedLimit = Persistent::GetSingleton().GTSGlobalSizeLimit.value;

                        _Frmt = fmt::format("Skill Based [{:.2f}x]", SkillBasedLimit);
                        *Scale = 0.0f;
                    }
                    else {
                        _Frmt = "%.1fx";
                    }

                    {

                        std::string ToolTip = fmt::format(
                            "Change the maximum player size\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x the limit is based on your skill level and perks.",
                            Max - 5.0f
                        );

                        ImUtil::SliderF("Max Player Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }

                }

                {   //Max Follower Size

                    float* Scale = &Settings.fMaxFollowerSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {

                        const float PlayersLimit = Persistent::GetSingleton().GTSGlobalSizeLimit.value;

                        if (PlayersLimit >= 250.01f) {
                            _Frmt = fmt::format("Based on Player [Infinite]", PlayersLimit);
                        }
                        else {
                            _Frmt = fmt::format("Based on Player [{:.2f}x]", PlayersLimit);
                        }

                        *Scale = 0.0f;
                    }
                    else {
                        _Frmt = "%.1fx";
                    }

                    {
                        std::string ToolTip = fmt::format(
                            "Change the maximum follower size\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x scale the limit will be based on the player's.",
                            Max - 5.0f
                        );

                        ImUtil::SliderF("Max Follower Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());

                    }
                }

                {   //Other NPC Max Size

                    float* Scale = &Settings.fMaxOtherSize;
                    const bool ShouldBeInf = *Scale > Max - 5.0f;
                    const bool ShouldBeAuto = *Scale < Min + 0.1f;

                    std::string _Frmt;
                    if (ShouldBeInf) {
                        _Frmt = "Infinite";
                        *Scale = 1000000.0f;
                    }
                    else if (ShouldBeAuto) {

                        const float NPCsLimit = Persistent::GetSingleton().GTSGlobalSizeLimit.value;

                        if (NPCsLimit >= 250.01f) {
                            _Frmt = fmt::format("Based on Player [Infinite]", NPCsLimit);
                        }
                        else {
                            _Frmt = fmt::format("Based on Player [{:.2f}x]", NPCsLimit);
                        }
                        *Scale = 0.0f;
                    }
                    else {
                        _Frmt = "%.1fx";
                    }

                    {
                        std::string ToolTip = fmt::format(
                            "Change the maximum size for non-follower NPCs\n"
                            "Higher than {:.0f}x scale disables the limit entirely\n"
                            "At 0.0x scale the limit will be based on the player's.",
                            Max - 5.0f
                        );
                        ImUtil::SliderF("Max NPC Size", Scale, Min, Max, ToolTip.c_str(), _Frmt.c_str());
                    }
                }
                ImGui::Spacing();
            }
        }

        ImUtil_Unique{

            const char* T0 = "Changes the amount of damage size-related actions do.";
            const char* T1 = "Changes the amount of damage increase regular melee atacks gain.";
            const char* T2 = "Adjust the speed at which you gain size experience.";
            const char* T3 = "Change the ammount of carry weight capacity gained based on your size.";

            if (ImUtil::ConditionalHeader("Multipiers", "Balance Mode Active", !Settings.bBalanceMode)) {

                ImUtil::SliderF("Size Damage Multiplier", &Settings.fSizeDamageMult, 0.1f, 2.0f, T0, "%.2fx");
                ImUtil::SliderF("Damage Multiplier", &Settings.fStatBonusDamageMult, 0.1f, 2.0f, T1, "%.2fx");
                ImUtil::SliderF("Carry Weight Multiplier", &Settings.fStatBonusCarryWeightMult, 0.1f, 2.0f, T3, "%.2fx");
                ImUtil::SliderF("Experience Multiplier", &Settings.fExpMult, 0.1f, 5.0f, T2, "%.2fx");
                ImGui::Spacing();

            }
        }
    }
}