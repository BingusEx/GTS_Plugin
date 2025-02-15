#include "managers/animation/Utils/AnimationUtils.hpp"
#include "managers/animation/Utils/CrawlUtils.hpp"
#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/damage/LaunchActor.hpp"
#include "managers/animation/Crawling.hpp"
#include "managers/GtsSizeManager.hpp"
#include "managers/CrushManager.hpp"
#include "utils/MovementForce.hpp"
#include "managers/audio/footstep.hpp"
#include "utils/actorUtils.hpp"
#include "data/persistent.hpp"
#include "managers/Rumble.hpp"
#include "Constants.hpp"
#include "rays/raycast.hpp"
#include "data/runtime.hpp"
#include "scale/scale.hpp"


namespace {
	void SpawnCrawlParticle(Actor* actor, float scale, NiPoint3 position) {
		SpawnParticle(actor, 4.60f, "GTS/Effects/Footstep.nif", NiMatrix3(), position, scale * 1.8f, 7, nullptr);
	}

	std::string_view GetImpactNode(CrawlEvent kind) {
		if (kind == CrawlEvent::RightKnee) {
			return "NPC R Calf [RClf]";
		} else if (kind == CrawlEvent::LeftKnee) {
			return "NPC L Calf [LClf]";
		} else if (kind == CrawlEvent::RightHand) {
			return "NPC R Finger20 [RF20]";
		} else if (kind == CrawlEvent::LeftHand) {
			return "NPC L Finger20 [LF20]";
		} else {
			return "NPC L Finger20 [LF20]";
		}
	}
}

namespace GTS {

	void DoCrawlingSounds(Actor* actor, float scale, NiAVObject* node, FootEvent foot_kind) { // Call crawling sounds
		if (actor) {
			auto profiler = Profilers::Profile("CrawlSounds");
			auto player = PlayerCharacter::GetSingleton();
			if (actor->formID == 0x14 && HasSMT(actor)) {
				scale *= 1.85f;
			}
			bool LegacySounds = Persistent::GetSingleton().legacy_sounds; // Determine if we should play old pre 2.00 update sounds
			if (scale > 1.2f && !actor->AsActorState()->IsSwimming()) {
				float movement = FootStepManager::Volume_Multiply_Function(actor, foot_kind);
				scale *= 0.75f;

				if (Runtime::GetBool("EnableGiantSounds")) {
					FootStepManager::PlayLegacySounds(movement, node, foot_kind, scale);
					return; // New Sounds are disabled for now
					if (!LegacySounds) {       // Play normal sounds
						FootStepManager::PlayNormalSounds(movement, node, foot_kind, scale);
						return;
					} else if (LegacySounds) { // Play old sounds
						FootStepManager::PlayLegacySounds(movement, node, foot_kind, scale);
						return;
					}
				}
			}
		}
	}
	void DoCrawlingFunctions(Actor* actor, float scale, float multiplier, float damage, CrawlEvent kind, std::string_view tag, float launch_dist, float damage_dist, float crushmult, DamageSource Cause) { // Call everything
		std::string_view name = GetImpactNode(kind);

		auto node = find_node(actor, name);
		if (!node) {
			return; // Make sure to return if node doesn't exist, no CTD in that case
		}

		float smt = 1.0f;
		float minimal_scale = 1.5f;

		LaunchActor::GetSingleton().LaunchAtCustomNode(actor, launch_dist, damage_dist, multiplier, node); // Launch actors
		// Order matters here since we don't want to make it even stronger during SMT, so that's why SMT check is after this function
		
		if (actor->formID == 0x14) {
			if (HasSMT(actor)) {
				smt = 2.0f; // Stronger Camera Shake
				multiplier *= 1.8f;
				minimal_scale = 1.0f;
				scale += 0.75f;
				damage *= 2.0f;
			}
		}

		//std::string rumbleName = std::format("{}{}", tag, actor->formID);
		std::string rumbleName = std::format("CrawlRumble_{}", actor->formID);
		Rumbling::Once(rumbleName, actor, Rumble_Crawl_KneeHand_Impact/2 * multiplier * smt, 0.02f, name, 0.0f); // Do Rumble

		DoDamageAtPoint(actor, damage_dist, damage, node, 20, 0.05f, crushmult, Cause); // Do size-related damage
		DoCrawlingSounds(actor, scale, node, FootEvent::Left);                      // Do impact sounds

		if (scale >= minimal_scale && !actor->AsActorState()->IsSwimming()) {
			NiPoint3 node_location = node->world.translate;

			NiPoint3 ray_start = node_location + NiPoint3(0.0f, 0.0f, meter_to_unit(-0.05f*scale)); // Shift up a little
			NiPoint3 ray_direction(0.0f, 0.0f, -1.0f);
			bool success = false;
			float ray_length = meter_to_unit(std::max(1.05f*scale, 1.05f));
			NiPoint3 explosion_pos = CastRay(actor, ray_start, ray_direction, ray_length, success);

			if (!success) {
				explosion_pos = node_location;
				explosion_pos.z = actor->GetPosition().z;
			}
			if (actor->formID == 0x14 && Runtime::GetBool("PCAdditionalEffects")) {
				SpawnCrawlParticle(actor, scale * multiplier, explosion_pos);
			}
			if (actor->formID != 0x14 && Runtime::GetBool("NPCSizeEffects")) {
				SpawnCrawlParticle(actor, scale * multiplier, explosion_pos);
			}
		}
	}

	void DoDamageAtPoint(Actor* giant, float radius, float damage, NiAVObject* node, float random, float bbmult, float crushmult, DamageSource Cause) { // Apply damage to specific bone
		auto profiler = Profilers::Profile("Other: CrawlDamage");
		if (!node) {
			return;
		}
		if (!giant) {
			return;
		}
		float giantScale = get_visual_scale(giant);

		float SCALE_RATIO = 1.25f;
		if (HasSMT(giant)) {
			SCALE_RATIO = 0.9f;
			giantScale *= 1.3f;
		}

		NiPoint3 NodePosition = node->world.translate;

		float maxDistance = radius * giantScale;
		float CheckDistance = 220 * giantScale;
		// Make a list of points to check

		if (IsDebugEnabled() && (giant->formID == 0x14 || IsTeammate(giant) || EffectsForEveryone(giant))) {
			DebugAPI::DrawSphere(glm::vec3(NodePosition.x, NodePosition.y, NodePosition.z), maxDistance, 300);
		}

		NiPoint3 giantLocation = giant->GetPosition();

		for (auto otherActor: find_actors()) {
			if (otherActor != giant) {
				float tinyScale = get_visual_scale(otherActor);
				if (giantScale / tinyScale > SCALE_RATIO) {
					NiPoint3 actorLocation = otherActor->GetPosition();
					if ((actorLocation-giantLocation).Length() <= CheckDistance) {
						
						int nodeCollisions = 0;
						float force = 0.0f;

						auto model = otherActor->GetCurrent3D();

						if (model) {
							VisitNodes(model, [&nodeCollisions, &force, NodePosition, maxDistance](NiAVObject& a_obj) {
								float distance = (NodePosition - a_obj.world.translate).Length() - Collision_Distance_Override;
								if (distance <= maxDistance) {
									nodeCollisions += 1;
									force = 1.0f - distance / maxDistance;
									return false;
								}
								return true;
							});
						}
						if (nodeCollisions > 0) {
							Utils_PushCheck(giant, otherActor, Get_Bone_Movement_Speed(giant, Cause)); 

							if (IsButtCrushing(giant) && !IsBeingEaten(otherActor) && GetSizeDifference(giant, otherActor, SizeType::VisualScale, false, true) > 1.2f) {
								PushActorAway(giant, otherActor, 1.0f);
							}
							
							CollisionDamage::GetSingleton().DoSizeDamage(giant, otherActor, damage, bbmult, crushmult, static_cast<int>(random), Cause, true);
						}
					}
				}
			}
		}
	}

	void ApplyAllCrawlingDamage(Actor* giant, int random, float bonedamage) { // Applies damage to all 4 crawl bones at once
		auto LC = find_node(giant, "NPC L Calf [LClf]");
		auto RC = find_node(giant, "NPC R Calf [RClf]");
		auto LH = find_node(giant, "NPC L Finger20 [LF20]");
		auto RH = find_node(giant, "NPC R Finger20 [RF20]");
		if (!LC) {
			return;
		}
		if (!RC) {
			return;
		}
		if (!LH) {
			return;
		}
		if (!RH) {
			return;
		} // CTD protection


		DoDamageAtPoint(giant, Radius_Crawl_KneeIdle, Damage_Crawl_Idle, LC, static_cast<float>(random), bonedamage, 2.5f, DamageSource::KneeIdleL);         // Call Left Calf
		DoDamageAtPoint(giant, Radius_Crawl_KneeIdle, Damage_Crawl_Idle, RC, static_cast<float>(random), bonedamage, 2.5f, DamageSource::KneeIdleR);        // Call Right Calf

		if (!IsTransferingTiny(giant)) { // Only do if we don't have someone in our left hand
			DoDamageAtPoint(giant, Radius_Crawl_HandIdle, Damage_Crawl_Idle, LH, static_cast<float>(random), bonedamage, 2.5f, DamageSource::HandIdleL); // Call Left Hand
		}

		DoDamageAtPoint(giant, Radius_Crawl_HandIdle, Damage_Crawl_Idle, RH, static_cast<float>(random), bonedamage, 2.5f, DamageSource::HandIdleR);    // Call Right Hand
	}
}