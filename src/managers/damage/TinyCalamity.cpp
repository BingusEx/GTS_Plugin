#include "managers/animation/Utils/CooldownManager.hpp"
#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/TinyCalamity.hpp"
#include "managers/InputManager.hpp"
#include "managers/CrushManager.hpp"
#include "magic/effects/common.hpp"
#include "managers/PerkHandler.hpp"
#include "managers/Attributes.hpp"
#include "utils/MovementForce.hpp"
#include "managers/highheel.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "ActionSettings.hpp"
#include "data/transient.hpp"
#include "utils/looting.hpp"
#include "managers/vore.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"
#include "UI/DebugAPI.hpp"


#include "profiler.hpp"

#include "node.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace {
    void ScareEnemies(Actor* giant)  {
		int FearChance = rand() % 2;
		if (FearChance <= 0) {
			Runtime::CastSpell(giant, giant, "GtsVoreFearSpell");
		}
	}

    void Task_WrathfulCalamityTask(Actor* giant, Actor* tiny, float duration) {
        ActorHandle tinyhandle = tiny->CreateRefHandle();
        ActorHandle gianthandle = giant->CreateRefHandle();
        std::string name = std::format("IRCalamity_{}", tiny->formID);

        auto node = find_node(tiny, "NPC Root [Root]");
		if (node) {
			SpawnParticle(tiny, 3.00, "GTS/gts_tinyrune.nif", NiMatrix3(), node->world.translate, 1.0, 7, node); 
		}
        Task_AdjustHalfLifeTask(tiny, 0.10, 3.0);
        SetBeingHeld(tiny, true);
        
        float Start = Time::WorldTimeElapsed();

        TaskManager::Run(name, [=](auto& progressData) {
			if (!tinyhandle) {
				return false;
			}
            if (!gianthandle) {
                return false;
            }
			Actor* tinyref = tinyhandle.get().get();
            Actor* giantref = gianthandle.get().get();
			float Finish = Time::WorldTimeElapsed();

            set_target_scale(tinyref, 0.03);

			if (get_visual_scale(tinyref) <= Minimum_Actor_Scale) {
                SetBeingHeld(tinyref, false);
                giantref->AsActorValueOwner()->RestoreActorValue(ACTOR_VALUE_MODIFIER::kDamage, ActorValue::kHealth, GetMaxAV(giantref, ActorValue::kHealth) * 0.075);
                ShrinkToNothingManager::Shrink(giantref, tinyref);
				return false;
			}
			return true;
		});
    }

    void PlayGoreEffects(Actor* giant, Actor* tiny) {
        if (!IsLiving(tiny)) {
            SpawnDustParticle(tiny, giant, "NPC Root [Root]", 3.0);
        } else {
            if (!LessGore()) {
                auto root = find_node(tiny, "NPC Root [Root]");
                if (root) {
                    float currentSize = get_visual_scale(tiny);
                    SpawnParticle(tiny, 0.60, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 1.25, 7, root);
                    SpawnParticle(tiny, 0.60, "GTS/Damage/Explode.nif", root->world.rotate, root->world.translate, currentSize * 1.25, 7, root);
                    SpawnParticle(tiny, 0.60, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 1.25, 7, root);
                    SpawnParticle(tiny, 0.60, "GTS/Damage/Crush.nif", root->world.rotate, root->world.translate, currentSize * 1.25, 7, root);
                    SpawnParticle(tiny, 1.20, "GTS/Damage/ShrinkOrCrush.nif", NiMatrix3(), root->world.translate, currentSize * 12.5, 7, root);
                }
            }
            Runtime::PlayImpactEffect(tiny, "GtsBloodSprayImpactSet", "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, true);
            Runtime::PlayImpactEffect(tiny, "GtsBloodSprayImpactSet", "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, true);
            Runtime::PlayImpactEffect(tiny, "GtsBloodSprayImpactSet", "NPC Root [Root]", NiPoint3{0, 0, -1}, 512, false, true);
            Runtime::CreateExplosion(tiny, get_visual_scale(tiny) * 0.5, "BloodExplosion");
        }
    }

    void RefreshDuration(Actor* giant) {
        if (Runtime::HasPerk(giant, "NoSpeedLoss")) {
            AttributeManager::GetSingleton().OverrideSMTBonus(0.75); // Reduce speed after crush
        } else {
            AttributeManager::GetSingleton().OverrideSMTBonus(0.35); // Reduce more speed after crush
        }
    }

    bool Collision_AllowTinyCalamityCrush(Actor* giant, Actor* tiny) {
        if (IsEssential(giant, tiny)) {
            return false;
        }
        float giantHp = GetAV(giant, ActorValue::kHealth);
		float tinyHp = GetAV(tiny, ActorValue::kHealth);

        float Multiplier = (get_visual_scale(giant) + 0.5) / get_visual_scale(tiny);

        if (giantHp >= ((tinyHp / Multiplier) * 1.25)) {
            return true;
        } else {
            return false;
        }
    }

    void FullSpeed_ApplyEffect(Actor* giant, float speed) {
        auto transient = Transient::GetSingleton().GetData(giant);
		if (transient) {
            bool& CanApplyEffect = transient->SMT_ReachedFullSpeed;
            if (speed < 1.0) {
                CanApplyEffect = true;
            } else if (speed >= 1.0 && CanApplyEffect) {
                CanApplyEffect = false;
                //Runtime::PlaySoundAtNode("TinyCalamity_ReachedSpeed", giant, 1.0, 1.0, "NPC COM [COM ]");
            } 
        }
    }
}

namespace Gts {
    bool TinyCalamity_WrathfulCalamity(Actor* giant) {
        bool perform = false;
        if (Runtime::HasPerkTeam(giant, "WrathfulCalamity") && HasSMT(giant) && !giant->IsSneaking()) {

            float threshold = 0.25;
            float level_bonus = std::clamp(GetGtsSkillLevel(giant) - 70.0f, 0.0f, 0.30f);
            threshold += level_bonus;

            float duration = 0.35;

            std::vector<Actor*> preys = Vore::GetSingleton().GetVoreTargetsInFront(giant, 1.0);
            bool OnCooldown = IsActionOnCooldown(giant, CooldownSource::Misc_TinyCalamityRage);
            for (auto tiny: preys) {
                if (tiny) {
                    float health = GetHealthPercentage(tiny);

                    float gts_hp = GetMaxAV(giant, ActorValue::kHealth);
                    float tiny_hp = GetMaxAV(tiny, ActorValue::kHealth);

                    float difference = std::clamp(gts_hp / tiny_hp, 0.5f, 2.0f);

                    threshold *= difference;

                    if (health <= threshold && !OnCooldown) {
                        if (IsBeingHeld(giant, tiny)) {
                            return false;
                        }
                        Task_WrathfulCalamityTask(giant, tiny, duration);
                        ApplyActionCooldown(giant, CooldownSource::Misc_TinyCalamityRage);
                        perform = true;
                    } else {
                        if (giant->formID == 0x14) {
                            if (!OnCooldown) {
                                std::string message = std::format("{} is too healthy for Wrathful Calamity", tiny->GetDisplayFullName());
                                Notify("Health: {:.0f}%; Requirement: {:.0f}%", health * 100.0, threshold * 100.0);
                                shake_camera(giant, 0.45, 0.30);
                                NotifyWithSound(giant, message);
                            } else {
                                std::string message = std::format("{} is on a cooldown: {:.1f} sec", "Wrathful Calamity", GetRemainingCooldown(giant, CooldownSource::Misc_TinyCalamityRage));
                                shake_camera(giant, 0.45, 0.30);
                                NotifyWithSound(giant, message);
                            }
                        }
                    }
                }
            }   
        }
        return perform;
    }

    void TinyCalamity_ShrinkActor(Actor* giant, Actor* tiny, float shrink) {
        auto profiler = Profilers::Profile("Calamity: Shrink");
        if (HasSMT(giant)) {
            bool HasPerk = Runtime::HasPerk(giant, "SmallMassiveThreatSizeSteal");
            float limit = Minimum_Actor_Scale;
            if (HasPerk) {
				DamageAV(giant, ActorValue::kHealth, -shrink * 1.25);
                shrink *= 1.25;
			}

            float target_scale = get_target_scale(tiny);

            if (target_scale > limit/GetSizeFromBoundingBox(tiny)) {
                if ((target_scale - shrink*0.0045) <= limit || target_scale <= limit) {
                    set_target_scale(tiny, limit);
                    return;
                }
                ShrinkActor(tiny, shrink * 0.0045, 0.0);
            } else { // cap it just in case
                set_target_scale(tiny, limit);
            }
        }
    }

    void TinyCalamity_SeekForShrink(Actor* giant, Actor* tiny, float damage, float maxFootDistance, DamageSource Cause, bool Right, bool ApplyCooldown, bool ignore_rotation) {
        std::vector<NiPoint3> CoordsToCheck = GetFootCoordinates(giant, Right, ignore_rotation);
        int nodeCollisions = 0;
        auto model = tiny->GetCurrent3D();
        if (model) {
            for (auto point: CoordsToCheck) {
                bool StopDamageLookup = false;
                if (!StopDamageLookup) {
                    VisitNodes(model, [&nodeCollisions, point, maxFootDistance, &StopDamageLookup](NiAVObject& a_obj) {
                        float distance = (point - a_obj.world.translate).Length() - Collision_Distance_Override;
                        if (distance <= maxFootDistance) {
                            StopDamageLookup = true;
                            nodeCollisions += 1;
                            return false;
                        }
                        return true;
                    });
                }
            }
            if (nodeCollisions > 0) {
                auto& CollisionDamage = CollisionDamage::GetSingleton();
                if (ApplyCooldown) { // Needed to fix Thigh Crush stuff
                    auto& sizemanager = SizeManager::GetSingleton();
                    bool OnCooldown = IsActionOnCooldown(tiny, CooldownSource::Damage_Thigh);
                    if (!OnCooldown) {
                        Utils_PushCheck(giant, tiny, Get_Bone_Movement_Speed(giant, Cause)); // pass original un-altered force
                        CollisionDamage.DoSizeDamage(giant, tiny, damage, 0.0, 10, 0, Cause, false);
                        ApplyActionCooldown(giant, CooldownSource::Damage_Thigh);
                    }
                } else {
                    Utils_PushCheck(giant, tiny, Get_Bone_Movement_Speed(giant, Cause)); // pass original un-altered force
                    CollisionDamage.DoSizeDamage(giant, tiny, damage, 0.0, 10, 0, Cause, false);
                }
            }
        }
    }

    void TinyCalamity_ExplodeActor(Actor* giant, Actor* tiny) {
        ModSizeExperience_Crush(giant, tiny, true);

        if (!tiny->IsDead()) {
            KillActor(giant, tiny);
        }

        PerkHandler::UpdatePerkValues(giant, PerkUpdate::Perk_LifeForceAbsorption);

        ActorHandle giantHandle = giant->CreateRefHandle();
        ActorHandle tinyHandle = tiny->CreateRefHandle();

        CrushBonuses(giant, tiny);                             // common.hpp
        PlayGoreEffects(tiny, giant);    
        MoveItems(giantHandle, tinyHandle, tiny->formID, DamageSource::Collision);

        Attacked(tiny, giant);
        
        PrintDeathSource(giant, tiny, DamageSource::Collision);

        float OldScale;
        giant->GetGraphVariableFloat("GiantessScale", OldScale); // save old scale
        giant->SetGraphVariableFloat("GiantessScale", 1.0); // Needed to allow Stagger to play, else it won't work

        shake_camera(giant, 1.7, 0.8);
        StaggerActor(giant, 0.5f);
        RefreshDuration(giant);

        Runtime::PlaySound("DefaultCrush", giant, 1.0, 1.0);

        if (tiny->formID != 0x14) {
            Disintegrate(tiny); // Set critical stage 4 on actors
        } else if (tiny->formID == 0x14) {
            TriggerScreenBlood(50);
            tiny->SetAlpha(0.0); // Player can't be disintegrated, so we make player Invisible
        }

        giant->SetGraphVariableFloat("GiantessScale", OldScale);
        Runtime::PlaySoundAtNode("TinyCalamity_Crush", giant, 1.0, 1.0, "NPC COM [COM ]");

        ScareEnemies(giant);
    }

    void TinyCalamity_StaggerActor(Actor* giant, Actor* tiny, float giantHp) { // when we can't crush the target
        float OldScale; 
        giant->GetGraphVariableFloat("GiantessScale", OldScale); // record old slace
        giant->SetGraphVariableFloat("GiantessScale", 1.0); // Needed to allow Stagger to play, else it won't work

        PushForward(giant, tiny, 800);
        AddSMTDuration(giant, 2.5);
        StaggerActor(giant, 0.5); // play stagger on the player

        Attacked(tiny, giant);

        DamageAV(tiny, ActorValue::kHealth, giantHp * 0.75);
        DamageAV(giant, ActorValue::kHealth, giantHp * 0.25);

        float hpcalc = (giantHp * 0.75f)/800.0;
        float xp = std::clamp(hpcalc, 0.0f, 0.12f);
        update_target_scale(tiny, -0.06, SizeEffectType::kShrink);
        ModSizeExperience(giant, xp);

        Runtime::PlaySoundAtNode("TinyCalamity_Impact", giant, 1.0, 1.0, "NPC COM [COM ]");
        shake_camera_at_node(giant, "NPC COM [COM ]", 16.0, 1.0);
        
        if (IsEssential(giant, tiny)) {
            Notify("{} is essential", tiny->GetDisplayFullName());
        } else {
            Notify("{} is too tough to be crushed", tiny->GetDisplayFullName());
        }

        giant->SetGraphVariableFloat("GiantessScale", OldScale);
        RefreshDuration(giant);
    }

    void TinyCalamity_SeekActors(Actor* giant) {
        auto profiler = Profilers::Profile("Calamity: SeekActor");
        if (giant->formID == 0x14) {
            if (giant->AsActorState()->IsSprinting() && HasSMT(giant)) {
                auto node = find_node(giant, "NPC Pelvis [Pelv]");
                if (!node) {
                    return;
                }
                NiPoint3 NodePosition = node->world.translate;

                float giantScale = get_visual_scale(giant);

                const float BASE_DISTANCE = 48.0;
                float CheckDistance = BASE_DISTANCE*giantScale;

                if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant))) {
                    DebugAPI::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), CheckDistance, 100, {0.0, 1.0, 1.0, 1.0});
                }

                NiPoint3 giantLocation = giant->GetPosition();
                for (auto otherActor: find_actors()) {
                    if (otherActor != giant) {
                        NiPoint3 actorLocation = otherActor->GetPosition();
                        if ((actorLocation - giantLocation).Length() < BASE_DISTANCE*giantScale*3) {
                            int nodeCollisions = 0;
                            float force = 0.0;

                            auto model = otherActor->GetCurrent3D();

                            if (model) {
                                VisitNodes(model, [&nodeCollisions, &force, NodePosition, CheckDistance](NiAVObject& a_obj) {
                                    float distance = (NodePosition - a_obj.world.translate).Length();
                                    if (distance < CheckDistance) {
                                        nodeCollisions += 1;
                                        force = 1.0 - distance / CheckDistance;
                                        return false;
                                    }
                                    return true;
                                });
                            }
                            if (nodeCollisions > 0) {
                                TinyCalamity_CrushCheck(giant, otherActor);
                            }
                        }
                    }
                }
            }
        }
    }

    void TinyCalamity_CrushCheck(Actor* giant, Actor* tiny) {
		auto profiler = Profilers::Profile("tinyCalamity: CrushCheck");
		if (giant == tiny) {
			return;
		}
        if (IsBeingHeld(giant, tiny)) { // Don't explode the ones in our hand
            return;
        }
		auto& persistent = Persistent::GetSingleton();
		if (persistent.GetData(giant)) {
			if (persistent.GetData(giant)->smt_run_speed >= 1.0) {
                float giantHp = GetAV(giant, ActorValue::kHealth);

				if (giantHp <= 0) {
					return; // just in case, to avoid CTD
				}

				if (Collision_AllowTinyCalamityCrush(giant, tiny)) {
                    StartCombat(tiny, giant);
                    TinyCalamity_ExplodeActor(giant, tiny);
				} else {
                    StartCombat(tiny, giant);
                    TinyCalamity_StaggerActor(giant, tiny, giantHp);
				}
			}
		}
	}

    void TinyCalamity_BonusSpeed(Actor* giant) { // Manages SMT bonus speed
		// Andy's TODO: Calc on demand rather than poll

		auto Attributes = Persistent::GetSingleton().GetData(giant);
		float Gigantism = 1.0 + (Ench_Aspect_GetPower(giant) * 0.25);

        float speed = 1.0; 
        float decay = 1.0;
        float cap = 1.0;

		float& currentspeed = Attributes->smt_run_speed;
		if (giant->AsActorState()->IsSprinting() && HasSMT(giant)) { // SMT Active and sprinting
			if (Runtime::HasPerk(giant, "NoSpeedLoss")) {
				speed = 1.25;
                decay = 1.5;
				cap = 1.25;
			}

			currentspeed += 0.004400 * speed * Gigantism * 5; // increase MS

			if (currentspeed > cap) {
				currentspeed = cap;
			}

            FullSpeed_ApplyEffect(giant, currentspeed);
		} else { // else decay bonus speed over time
			if (currentspeed > 0.0) {
				currentspeed -= (0.045000 * 5) / decay;
			} else if (currentspeed <= 0.0) {
				currentspeed = 0.0;
			} 
		}
    }
}