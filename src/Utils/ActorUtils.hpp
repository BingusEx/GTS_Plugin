#pragma once

namespace GTS {

	enum class AnimationCondition {
		kHugs = 0,
		kStompsAndKicks = 1,
		kGrabAndSandwich = 2,
		kVore = 3,
		kOthers = 4,
	};

	float GetPerkBonus_OnTheEdge(Actor* giant, float amt);

	[[nodiscard]] RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis);

	RE::Actor* GetActorPtr(RE::Actor* actor);

	RE::Actor* GetActorPtr(RE::Actor& actor);

	RE::Actor* GetActorPtr(RE::ActorHandle& actor);

	RE::Actor* GetActorPtr(const RE::ActorHandle& actor);

	RE::Actor* GetActorPtr(RE::FormID formId);

	RE::Actor* GetCharContActor(RE::bhkCharacterController* charCont);

	void Task_AdjustHalfLifeTask(RE::Actor* tiny, float halflife, double revert_after);

	float GetLaunchPower(RE::Actor* giant, float sizeRatio);

	void StartResetTask(RE::Actor* tiny);
	void PlayMoanSound(RE::Actor* actor, float volume);
	void PlayLaughSound(RE::Actor* actor, float volume, int type);

	// GTS State Bools
	void Potion_SetMightBonus(RE::Actor* giant, float value, bool add);
	float Potion_GetMightBonus(RE::Actor* giant);

	float Potion_GetSizeMultiplier(RE::Actor* giant);

	void Potion_SetShrinkResistance(RE::Actor* giant, float value);
	float Potion_GetShrinkResistance(RE::Actor* giant);

	void Potion_SetUnderGrowth(RE::Actor* actor, bool set);
	bool Potion_IsUnderGrowthPotion(RE::Actor* actor);
	
	bool IsInsect(RE::Actor* actor, bool performcheck);
	bool IsFemale(RE::Actor* a_Actor, bool AllowOverride = false);
	bool IsDragon(RE::Actor* actor);
	bool IsGiant(RE::Actor* actor);
	bool IsMammoth(RE::Actor* actor);
	bool IsLiving(RE::Actor* actor);
	bool IsUndead(RE::Actor* actor, bool PerformCheck);
	bool WasReanimated(RE::Actor* actor);
	bool IsFlying(RE::Actor* actor);
	bool IsHostile(RE::Actor* giant, RE::Actor* tiny);
	bool CanPerformAnimationOn(RE::Actor* giant, RE::Actor* tiny, bool HugCheck);
	bool IsEssential(RE::Actor* giant, RE::Actor* actor);
	bool IsHeadtracking(RE::Actor* giant);
	bool AnimationsInstalled(RE::Actor* giant);
	bool IsInGodMode(RE::Actor* giant);
	bool IsFreeCameraEnabled();
	bool IsDebugEnabled();
	bool CanDoDamage(RE::Actor* giant, RE::Actor* tiny, bool HoldCheck);

	void Attachment_SetTargetNode(RE::Actor* giant, AttachToNode Node);
	AttachToNode Attachment_GetTargetNode(RE::Actor* giant);

	void SetBusyFoot(RE::Actor* giant, BusyFoot Foot);
	BusyFoot GetBusyFoot(RE::Actor* giant);
	
	void ControlAnother(RE::Actor* target, bool reset);
	RE::Actor* GetPlayerOrControlled();

	void RecordSneaking(RE::Actor* actor);
	void SetSneaking(RE::Actor* actor, bool override_sneak, int enable);

	void SetWalking(RE::Actor* actor, int enable);

	// Gts Bools end

	// GTS Actor Functions
	float GetDamageSetting();
	float GetFallModifier(RE::Actor* giant);

	std::vector<RE::Actor*> GetMaxActionableTinyCount(RE::Actor* giant, const std::vector<RE::Actor*>& actors);

	float Ench_Aspect_GetPower(RE::Actor* giant);
	float Ench_Hunger_GetPower(RE::Actor* giant);

	float Perk_GetSprintShrinkReduction(RE::Actor* actor);

	float GetDamageResistance(RE::Actor* actor);
	float GetDamageMultiplier(RE::Actor* actor);
	float Damage_CalculateSizeDamage(RE::Actor* giant, RE::Actor* tiny);

	float GetSizeDifference(Actor* giant, Actor* tiny, SizeType Type, bool Check_SMT, bool HH);
	float GetActorGTSWeight(Actor* giant);
	float GetActorGTSHeight(Actor* giant);
	float GetSizeFromBoundingBox(Actor* tiny);
	float GetRoomStateScale(Actor* giant);
	float GetProneAdjustment();

	void override_actor_scale(Actor* giant, float amt, SizeEffectType type);
	void update_target_scale(Actor* giant, float amt, SizeEffectType type);
	float get_update_target_scale(Actor* giant, float amt, SizeEffectType type);

	void SpawnActionIcon(Actor* giant);
	// End

	// GTS State Controllers
	void SetBeingHeld(Actor* tiny, bool enable);
	void SetProneState(Actor* giant, bool enable);
	void SetBetweenBreasts(Actor* actor, bool enable);
	void SetBeingEaten(Actor* tiny, bool enable);
	void SetBeingGrinded(Actor* tiny, bool enable);
	void SetCameraOverride(Actor* actor, bool enable);

	bool IsUsingAlternativeStomp(Actor* giant);

	void SetReanimatedState(Actor* actor);
	void ShutUp(Actor* actor);

	// GTS State Controllers end
	void PlayAnimation(Actor* actor, std::string_view animName);

	void DecreaseShoutCooldown(Actor* giant);

	void Disintegrate(Actor* actor);
	void UnDisintegrate(Actor* actor);

	void SetRestrained(Actor* actor);
	void SetUnRestrained(Actor* actor);

	void SetDontMove(Actor* actor);
	void SetMove(Actor* actor);

	void ForceRagdoll(Actor* actor, bool forceOn);

	std::vector<hkpRigidBody*> GetActorRB(Actor* actor);
	void PushActorAway(Actor* source_act, Actor* receiver, float force);
	void KnockAreaEffect(TESObjectREFR* source, float afMagnitude, float afRadius);
	void ApplyManualHavokImpulse(Actor* target, float afX, float afY, float afZ, float Multiplier);

	void CompleteDragonQuest(Actor* tiny, ParticleType Type, bool dead);

	float get_distance_to_actor(Actor* receiver, Actor* target);
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply);
	float GetHighHeelsBonusDamage(Actor* actor, bool multiply, float adjust);

	void EnableFreeCamera();

	bool DisallowSizeDamage(Actor* giant, Actor* tiny);
	bool AllowDevourment();
	bool AllowCameraTracking();
	bool LessGore();

	bool IsTeammate(Actor* actor);
	bool EffectsForEveryone(Actor* giant);
	
	bool BehaviorGraph_DisableHH(Actor* actor);
	bool IsEquipBusy(Actor* actor);
	
	bool IsRagdolled(Actor* actor);
	bool IsGrowing(Actor* actor);
	bool IsChangingSize(Actor* actor);

	bool IsFootGrinding(Actor* actor);
	bool IsProning(Actor* actor);
	bool IsCrawling(Actor* actor);
	bool IsHugCrushing(Actor* actor);
	bool IsHugHealing(Actor* actor);
	bool IsVoring(Actor* giant);
	bool IsHuggingFriendly(Actor* actor);
	bool IsTransitioning(Actor* actor);
	bool IsJumping(Actor* actor);
	bool IsBeingHeld(Actor* giant, Actor* tiny);
	bool IsBetweenBreasts(Actor* actor);
	bool IsTransferingTiny(Actor* actor);
	bool IsUsingThighAnimations(Actor* actor);
	bool IsSynced(Actor* actor);
	bool CanDoPaired(Actor* actor);
	bool IsThighCrushing(Actor* actor);
	bool IsThighSandwiching(Actor* actor);
	bool IsBeingEaten(Actor* tiny);
	bool IsGtsBusy(Actor* actor);

	bool IsStomping(Actor* actor);
	bool IsInCleavageState(Actor* actor);
	bool IsCleavageZIgnored(Actor* actor);
	bool IsInsideCleavage(Actor* actor);
	bool IsKicking(Actor* actor);
	bool IsTrampling(Actor* actor);

	bool CanDoCombo(Actor* actor);
	bool IsCameraEnabled(Actor* actor);
	bool IsCrawlVoring(Actor* actor);
	bool IsButtCrushing(Actor* actor);
	bool ButtCrush_IsAbleToGrow(Actor* actor, float limit);
	bool IsBeingGrinded(Actor* actor);
	bool IsHugging(Actor* actor);
	bool IsBeingHugged(Actor* actor); 
	bool CanDoButtCrush(Actor* actor, bool apply_cooldown);
	bool GetCameraOverride(Actor* actor);
	// GTS State Bools End

	// Gts Bools
	bool IsGrowthSpurtActive(Actor* actor);
	bool HasGrowthSpurt(Actor* actor);
	bool InBleedout(Actor* actor);
	bool AllowStagger(Actor* giant, Actor* tiny);
	bool IsMechanical(Actor* actor);
	bool IsHuman(Actor* actor);
	bool IsBlacklisted(Actor* actor);

    bool IsGtsTeammate(Actor* actor);

	void ResetCameraTracking();
	void CallDevourment(Actor* giant, Actor* tiny);
	void GainWeight(Actor* giant, float value);
	void CallVampire();
	void CallHelpMessage();
	void AddCalamityPerk();
	void AddPerkPoints(float a_Level);

	void AddStolenAttributes(Actor* giant, float value);
	void AddStolenAttributesTowards(Actor* giant, ActorValue type, float value);
	float GetStolenAttributes_Values(Actor* giant, ActorValue type);
	float GetStolenAttributes(Actor* giant);
	void DistributeStolenAttributes(Actor* giant, float value);

	float GetRandomBoost();
	
	float GetButtCrushCost(Actor* actor, bool DoomOnly);
	float Perk_GetCostReduction(Actor* giant);
	float GetAnimationSlowdown(Actor* giant);

	void DoFootstepSound(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	void DoDustExplosion(Actor* giant, float modifier, FootEvent kind, std::string_view node);
	void SpawnParticle(Actor* actor, float lifetime, const char* modelName, const NiMatrix3& rotation, const NiPoint3& position, float scale, std::uint32_t flags, NiAVObject* target);
	void SpawnDustParticle(Actor* giant, Actor* tiny, std::string_view node, float size);

	void Utils_PushCheck(Actor* giant, Actor* tiny, float force);

	void StaggerOr(Actor* giant, Actor* tiny, float afX, float afY, float afZ, float afMagnitude);
	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause);
	void DoDamageEffect(Actor* giant, float damage, float radius, int random, float bonedamage, FootEvent kind, float crushmult, DamageSource Cause, bool ignore_rotation);

	void PushTowards(Actor* giantref, Actor* tinyref, std::string_view bone, float power, bool sizecheck);
	void PushTowards_Task(const ActorHandle& giantHandle, const ActorHandle& tinyHandle, const NiPoint3& startCoords, const NiPoint3& endCoords, std::string_view TaskName, float power, bool sizecheck);
	void PushTowards(Actor* giantref, Actor* tinyref, NiAVObject* bone, float power, bool sizecheck);
	void PushForward(Actor* giantref, Actor* tinyref, float power);
	void TinyCalamityExplosion(Actor* giant, float radius);
	void ShrinkOutburst_Shrink(Actor* giant, Actor* tiny, float shrink, float gigantism);
	void ShrinkOutburstExplosion(Actor* giant, bool WasHit);

	void Utils_ProtectTinies(bool Balance);
	void LaunchImmunityTask(Actor* giant, bool Balance);

	bool HasSMT(Actor* giant);
	void NotifyWithSound(Actor* actor, std::string_view message);

	hkaRagdollInstance* GetRagdoll(Actor* actor);

	void DisarmActor(Actor* tiny, bool drop);
	void ManageRagdoll(Actor* tinyref, float deltaLength, NiPoint3 deltaLocation, NiPoint3 targetLocation);
	void ChanceToScare(Actor* giant, Actor* tiny, float duration, int random, bool apply_sd);
	void StaggerActor(Actor* receiver, float power);
	void StaggerActor(Actor* giant, Actor* tiny, float power);
	void StaggerActor_Around(Actor* giant, const float radius, bool launch);
	

	float GetMovementModifier(Actor* giant);
	float GetGtsSkillLevel(Actor* giant);
	float GetXpBonus();

	void DragonAbsorptionBonuses();

	void AddSMTDuration(Actor* actor, float duration, bool perk_check = true);
	void AddSMTPenalty(Actor* actor, float penalty);

	void PrintDeathSource(Actor* giant, Actor* tiny, DamageSource cause);
	void PrintSuffocate(Actor* pred, Actor* prey);
	void ShrinkUntil(Actor* giant, Actor* tiny, float expected, float halflife, bool animation);
	void DisableCollisions(Actor* actor, TESObjectREFR* otherActor);
	void EnableCollisions(Actor* actor);

	void SpringGrow(Actor* actor, float amt, float halfLife, std::string_view naming, bool drain);
	void SpringShrink(Actor* actor, float amt, float halfLife, std::string_view naming);

	void ResetGrab(Actor* giant);
	void FixAnimationsAndCamera();

	bool CanPerformAnimation(Actor* giant, AnimationCondition type);
	void AdvanceQuestProgression(Actor* giant, Actor* tiny, QuestStage stage, float value, bool vore);
	float GetQuestProgression(int stage);
	void ResetQuest();

	void SpawnHearts(Actor* giant, Actor* tiny, float Z, float scale, bool hugs);
	void SpawnCustomParticle(Actor* actor, ParticleType Type, NiPoint3 spawn_at_point, std::string_view spawn_at_node, float scale_mult);

	void InflictSizeDamage(Actor* attacker, Actor* receiver, float value);

	float Sound_GetFallOff(NiAVObject* source, float mult);

	// RE Fun:
	void SetCriticalStage(Actor* actor, int stage);
	void Attacked(Actor* victim, Actor* agressor);
	void StartCombat(Actor* victim, Actor* agressor);
  	void ApplyDamage(Actor* giant, Actor* tiny, float damage);
	void SetObjectRotation_X(TESObjectREFR* ref, float X);
	void StaggerActor_Directional(Actor* giant, float power, Actor* tiny);
	void SetLinearImpulse(bhkRigidBody* body, const hkVector4& a_impulse);
	void SetAngularImpulse(bhkRigidBody* body, const hkVector4& a_impulse);

	std::int16_t GetItemCount(InventoryChanges* changes, RE::TESBoundObject* a_obj);
	int GetCombatState(Actor* actor);
	bool IsMoving(Actor* giant);

	bool IsPlayerFirstPerson(Actor* a_actor);

	void ForEachReferenceInRange_Custom(RE::TESObjectREFR* origin, float radius, std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR& ref)> callback);

}
