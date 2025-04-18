#include "Papyrus/TotalControl.hpp"
#include "Magic/Effects/Common.hpp"
#include "Managers/Rumble.hpp"

using namespace GTS;
using namespace RE::BSScript;

namespace {

	constexpr std::string_view PapyrusClass = "GTSControl";

	void GrowTeammate(StaticFunctionTag*, float power) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			if (Runtime::HasPerkTeam(casterRef, "GTSPerkGrowthDesireAug")) {
				for (auto targetRef: FindTeammates()) {
					if (targetRef) {
						float falloff = 0.11f * get_visual_scale(targetRef);
						float Volume = std::clamp(get_visual_scale(targetRef)/8.0f, 0.20f, 1.0f);
						Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", targetRef, Volume, 1.0f, "NPC Pelvis [Pelv]", falloff);

						// Thread safe handles
						ActorHandle casterHandle = casterRef->CreateRefHandle();
						ActorHandle targetHandle = targetRef->CreateRefHandle();

						std::string name = std::format("GrowFollower_{}", targetRef->formID);
						constexpr float DURATION = 2.0f;

						TaskManager::RunFor(name, DURATION, [=](auto& progressData){
							if (!casterHandle) {
								return false;
							}
							if (!targetHandle) {
								return false;
							}
							float timeDelta = static_cast<float>(progressData.delta * 60); // Was optimised as 60fps

							auto target = targetHandle.get().get();
							auto caster = casterHandle.get().get();

							float target_scale = get_target_scale(target);
							float magicka = std::clamp(GetMagikaPercentage(caster), 0.05f, 1.0f);

							float bonus = 1.0f;
							if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
								bonus = target_scale * 0.25f + 0.75f;
							}

							DamageAV(caster, ActorValue::kMagicka, 0.45f * (target_scale * 0.25f + 0.75f) * magicka * bonus * timeDelta * power);
							Grow(target, 0.0030f * magicka * bonus, 0.0f);
							Rumbling::Once("GrowOtherButton", target, 1.0f, 0.05f);

							return true;
						});
					}
				}
			}
		}
		
	}

	void ShrinkTeammate(StaticFunctionTag*, float power) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			if (Runtime::HasPerkTeam(casterRef, "GTSPerkGrowthDesireAug")) {
				for (auto targetRef: FindTeammates()) {
					if (targetRef) {
						float scale = get_visual_scale(targetRef);
						float Volume = std::clamp(scale * 0.10f, 0.10f, 1.0f);
						
						float falloff = 0.11f * scale;
						Runtime::PlaySoundAtNode_FallOff("GTSSoundShrink", targetRef, Volume, 1.0f, "NPC Pelvis [Pelv]", falloff);
						
						// Thread safe handles
						ActorHandle casterHandle = casterRef->CreateRefHandle();
						ActorHandle targetHandle = targetRef->CreateRefHandle();

						constexpr float DURATION = 2.0f;
						std::string name = std::format("ShrinkFollower_{}", targetRef->formID);

						TaskManager::RunFor(name, DURATION, [=](auto& progressData){
							if (!casterHandle) {
								return false;
							}
							if (!targetHandle) {
								return false;
							}

							auto target = targetHandle.get().get();
							auto caster = casterHandle.get().get();

							float target_scale = get_target_scale(target);
							float magicka = std::clamp(GetMagikaPercentage(caster), 0.05f, 1.0f);

							float bonus = 1.0f;
							if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
								bonus = target_scale * 0.25f + 0.75f;
							}

							if (target_scale > get_natural_scale(target, true)) {
								DamageAV(caster, ActorValue::kMagicka, 0.25f * (target_scale * 0.25f + 0.75f) * magicka * bonus * TimeScale() * power);
								ShrinkActor(target, 0.0030f * magicka * bonus, 0.0f);
								Rumbling::Once("ShrinkOtherButton", target, 1.0f, 0.05f);
							}
							return true;
						});
					}
				}
			}
		}
	}

	void GrowPlayer(StaticFunctionTag*, float power) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			if (Runtime::HasPerkTeam(casterRef, "GTSPerkGrowthDesireAug")) {
				float scale = get_visual_scale(casterRef);
				float Volume = std::clamp(scale * 0.20f, 0.20f, 1.0f);

				float falloff = 0.11f * scale;

				Runtime::PlaySoundAtNode_FallOff("GTSSoundGrowth", casterRef, Volume, 1.0f, "NPC Pelvis [Pelv]", falloff);
			
				// Thread safe handles
				ActorHandle casterHandle = casterRef->CreateRefHandle();

				constexpr float DURATION = 2.0f;
				std::string name = std::format("GrowPlayer_{}", casterRef->formID);

				TaskManager::RunFor(name, DURATION, [=](auto& progressData){
					if (!casterHandle) {
						return false;
					}

					auto caster = casterHandle.get().get();

					float caster_scale = get_visual_scale(caster);
					float target_scale = get_target_scale(caster);

					float bonus = 1.0f;
					if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
						bonus = target_scale * 0.25f + 0.75f;
					}

					float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);
					DamageAV(caster, ActorValue::kStamina, 0.45f * (caster_scale * 0.5f + 0.5f) * stamina * TimeScale());

					Grow(caster, 0.0030f * stamina, 0.0f);

					Rumbling::Once("GrowButton", caster, 1.0f, 0.05f);

					return true;
				});
			}
		}
	}
	

	void ShrinkPlayer(StaticFunctionTag*, float power) {
		auto casterRef = PlayerCharacter::GetSingleton();
		if (casterRef) {
			if (Runtime::HasPerkTeam(casterRef, "GTSPerkGrowthDesireAug")) {
				float scale = get_visual_scale(casterRef);
				float Volume = std::clamp(scale * 0.10f, 0.10f, 1.0f);
				float falloff = 0.11f * scale;

				Runtime::PlaySoundAtNode_FallOff("GTSSoundShrink", casterRef, Volume, 1.0f, "NPC Pelvis [Pelv]", falloff);
			
				// Thread safe handles
				ActorHandle casterHandle = casterRef->CreateRefHandle();

				constexpr float DURATION = 2.0f;
				std::string name = std::format("ShrinkPlayer_{}", casterRef->formID);

				TaskManager::RunFor(name, DURATION, [=](auto& progressData){
					if (!casterHandle) {
						return false;
					}

					auto caster = casterHandle.get().get();

					float caster_scale = get_visual_scale(caster);
					float target_scale = get_target_scale(caster);

					float stamina = std::clamp(GetStaminaPercentage(caster), 0.05f, 1.0f);

					float bonus = 1.0f;
					if (Runtime::HasMagicEffect(caster, "GTSPotionEffectSizeAmplify")) {
						bonus = target_scale * 0.25f + 0.75f;
					}

					if (target_scale > Minimum_Actor_Scale) {
						DamageAV(caster, ActorValue::kStamina, 0.25f * (caster_scale * 0.5f + 0.5f) * stamina * TimeScale());
						ShrinkActor(caster, 0.0020f * stamina, 0.0f);
						Rumbling::Once("ShrinkButton", caster, 0.60f, 0.05f);
					} else {
						set_target_scale(caster, Minimum_Actor_Scale);
						return false;
					}
					return true;
				});
			}
		}
	}

	void CallRapidGrowth(StaticFunctionTag*, float amt, float halflife) {
		auto player = PlayerCharacter::GetSingleton();
		if (player) {
			if (Runtime::HasPerkTeam(player, "GTSPerkGrowthDesireAug")) {
				float target = get_target_scale(player);
				float max_scale = get_max_scale(player);// * get_natural_scale(player);
				if (target >= max_scale) {
					NotifyWithSound(player, "You can't grow any further");
					shake_camera(player, 0.45f, 0.30f);
					return;
				}
				SpringGrow(player, amt, halflife, "Input_G", true);
			}
		}
	}

	void CallRapidShrink(StaticFunctionTag*, float amt, float halflife) {
		auto player = PlayerCharacter::GetSingleton();
		if (player) {
			if (Runtime::HasPerkTeam(player, "GTSPerkGrowthDesireAug")) {
				float target = get_target_scale(player);
				if (target <= Minimum_Actor_Scale) {
					NotifyWithSound(player, "You can't shrink any further");
					shake_camera(player, 0.45f, 0.30f);
					return;
				}
				SpringShrink(player, amt, halflife, "Input_S");

			}
		}
	}

}

namespace GTS {

	bool register_total_control(IVirtualMachine* vm) {

		//Followers
		vm->RegisterFunction("GrowTeammate", PapyrusClass, GrowTeammate);
		vm->RegisterFunction("ShrinkTeammate", PapyrusClass, ShrinkTeammate);

		//Player
		vm->RegisterFunction("GrowPlayer", PapyrusClass, GrowPlayer);
		vm->RegisterFunction("ShrinkPlayer", PapyrusClass, ShrinkPlayer);

		//Rappid Growth
		vm->RegisterFunction("CallRapidGrowth", PapyrusClass, CallRapidGrowth);
		vm->RegisterFunction("CallRapidShrink", PapyrusClass, CallRapidShrink);

		return true;
	}

}