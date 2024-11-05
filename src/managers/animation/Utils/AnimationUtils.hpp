#pragma once

#include "events.hpp"
#include "managers/animation/Utils/AttachPoint.hpp"
#include "managers/animation/Utils/TurnTowards.hpp"

using namespace std;
using namespace SKSE;
using namespace RE;
using namespace Gts;

namespace Gts {
	enum class FollowerAnimType {
		ThighSandwich,
		ButtCrush,
		Grab,
		Hugs,
		Vore,
	};

	void BlockFirstPerson(Actor* actor, bool block);
	void Task_ApplyAbsorbCooldown(Actor* giant);
	
	void Anims_FixAnimationDesync(Actor* giant, Actor* tiny, bool reset);
	void ForceFollowerAnimation(Actor* giant, FollowerAnimType Type);
	void Vore_AttachToRightHandTask(Actor* giant, Actor* tiny);

	void UpdateFriendlyHugs(Actor* giant, Actor* tiny, bool force);

	bool Vore_ShouldAttachToRHand(Actor* giant, Actor* tiny);

	void HugCrushOther(Actor* giant, Actor* tiny);

	void AbortHugAnimation(Actor* giant, Actor* tiny);
	void Utils_UpdateHugBehaviors(Actor* giant, Actor* tiny);
	void Utils_UpdateHighHeelBlend(Actor* giant, bool reset);
	void Task_HighHeel_SyncVoreAnim(Actor* giant);
	void StartHealingAnimation(Actor* giant, Actor* tiny);

	void AllowToDoVore(Actor* actor, bool toggle);
	void AllowToBeCrushed(Actor* actor, bool toggle);
	void ManageCamera(Actor* giant, bool enable, CameraTracking type);
	void ApplyButtCrushCooldownTask(Actor* giant);

	void LaunchTask(Actor* actor, float radius, float power, FootEvent kind);

	void DoLaunch(Actor* giant, float radius, float damage, FootEvent kind);
	void DoLaunch(Actor* giant, float radius, float damage, NiAVObject* node);

	void GrabStaminaDrain(Actor* giant, Actor* tiny, float sizedifference);
	void DrainStamina(Actor* giant, std::string_view TaskName, std::string_view perk, bool enable, float power);

	void SpawnHurtParticles(Actor* giant, Actor* grabbedActor, float mult, float dustmult);

	void AdjustFacialExpression(Actor* giant, int ph, float target, std::string_view type);
	void AdjustFacialExpression(Actor* giant, int ph, float target, float speed_phenome, float speed_modifier, std::string_view type);

	float GetWasteMult(Actor* giant);
	float GetPerkBonus_Basics(Actor* Giant);
	float GetPerkBonus_Thighs(Actor* Giant);

	void DoFootGrind(Actor* giant, Actor* tiny);
	void DoFingerGrind(Actor* giant, Actor* tiny);
	void FingerGrindCheck(Actor* giant, CrawlEvent kind, bool Right, float radius);
	void FootGrindCheck(Actor* actor, float radius, bool strong, bool Right);

	void DoDamageAtPoint_Cooldown(Actor* giant, float radius, float damage, NiAVObject* node, NiPoint3 NodePosition, float random, float bbmult, float crushmult, float pushpower, DamageSource Cause);
	void ApplyThighDamage(Actor* actor, bool right, bool CooldownCheck, float radius, float damage, float bbmult, float crush_threshold, int random, DamageSource Cause);
	void ApplyFingerDamage(Actor* giant, float radius, float damage, NiAVObject* node, float random, float bbmult, float crushmult, float Shrink, DamageSource Cause);

	std::vector<NiPoint3> GetThighCoordinates(Actor* giant, std::string_view calf, std::string_view feet, std::string_view thigh);
	std::vector<NiPoint3> GetFootCoordinates(Actor* actor, bool Right, bool ignore_rotation);
	NiPoint3 GetHeartPosition(Actor* giant, Actor* tiny, bool hugs);

	void AbsorbShout_BuffCaster(Actor* giantref, Actor* tinyref);
	void Task_TrackSizeTask(Actor* giant, Actor* tiny, std::string_view naming, bool check_ticks, float time_mult);
	void Task_FacialEmotionTask_OpenMouth(Actor* giant, float duration, std::string_view naming);
	void Task_FacialEmotionTask_Moan(Actor* giant, float duration, std::string_view naming);
	void Task_FacialEmotionTask_Smile(Actor* giant, float duration, std::string_view naming);

	void Laugh_Chance(Actor* giant, Actor* otherActor, float multiply, std::string_view name);
	void Laugh_Chance(Actor* giant, float multiply, std::string_view name);

	float GetHugStealRate(Actor* actor);
	float GetHugShrinkThreshold(Actor* actor);
	float GetHugCrushThreshold(Actor* giant, Actor* tiny, bool check_size);
}
