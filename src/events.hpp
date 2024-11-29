#pragma once

#include "profiler.hpp"

using namespace std;
using namespace RE;
using namespace SKSE;

namespace Gts {
	
	enum class FootEvent {
		Left,
		Right,
		Front,
		Back,
		JumpLand,
		Unknown,
		Butt,
		Breasts,
	};

	enum class SizeType {
		GiantessScale,
		VisualScale,
		TargetScale,
	};
	enum class ParticleType {
		Red,
		DarkRed,
		Green,
		Blue,
		Hearts,
		None,
	};

	enum class QuestStage {
		HugSteal,
		HugSpellSteal,
		Crushing,
		ShrinkToNothing,
		HandCrush,
		Vore,
		Giant,
		Dragon,
	};

	enum class AttachToNode {
		None,
		Cleavage,
		ObjectA,
		ObjectB,
		ObjectL,
	};

	enum class ShrinkSource {
		Other,
		Magic,
		Hugs,
		Enchantment,
	};

	enum class SizeEffectType {
		kShrink,
		kGrow,
		kSteal,
		kGift,
		kNeutral,
	};

	enum class SizeAttribute {
		Normal,
		Sprint,
		JumpFall,
		HighHeel,
	};
	
	enum class CameraTracking_MCM {
		None,
		Spine,
		Clavicle,
		Breasts_01,
		Breasts_02,
		Breasts_03, // 3BBB tip
		Neck,
		Butt,
	};

	enum class CameraTracking {
		None,
		Butt,
		Knees,
		Breasts_02,
		Thigh_Crush,
		Thigh_Sandwich,
		Hand_Right,
		Hand_Left,
		Grab_Left,
		L_Foot,
		R_Foot,
		Mid_Butt_Legs,
		VoreHand_Right,
		Finger_Right,
		Finger_Left,
		ObjectA,
		ObjectB,
	};

	enum class NodeMovementType {
		Movement_LeftLeg,
		Movement_RightLeg,
		Movement_RightHand,
		Movement_LeftHand,
		Movement_None,
	};

	enum class DamageSource {
		HandCrawlRight,
		HandCrawlLeft,
		HandIdleL,
		HandIdleR,
		KneeIdleL,
		KneeIdleR,
		HandDropRight,
		HandDropLeft,
		HandSwipeRight,
		HandSwipeLeft,
		HandSlamRight,
		HandSlamLeft,
		RightFinger,
		RightFinger_Impact,
		LeftFinger,
		LeftFinger_Impact,
		KickedRight,
		KickedLeft,
		KneeDropRight,
		KneeDropLeft,
		KneeRight,
		KneeLeft,
		Breast,
		BreastImpact,
		Hugs,
		Booty,
		BodyCrush,
		Vored,
		Spell,
		Melted,
		Explode,
		Crushed,
		CrushedRight,
		FootIdleR,
		WalkRight,
		WalkLeft,
		CrushedLeft,
		FootIdleL,
		Overkill,
		Shrinked,
		Collision,
		FootGrindedRight,
		FootGrindedLeft,
		FootGrindedRight_Impact,
		FootGrindedLeft_Impact,
		HandCrushed,
		ThighSandwiched,
		ThighCrushed,
		BlockDamage,
		HitSteal,
	};

	enum class GrowthAnimation {
		None,
		Growth_1,
		Growth_2,
		Growth_3,
		Growth_4,
		Growth_5,
		Growth_6,
	};

	enum class CrawlEvent {
		LeftKnee,
		RightKnee,
		LeftHand,
		RightHand,
	};

	enum class PerkUpdate {
		Perk_Acceleration,
		Perk_LifeForceAbsorption,
		Perk_None,
	};

	enum class CharEmotionType {
		Phenome,
		Modifier,
		Expression,
	};

	struct Impact {
		Actor* actor;
		FootEvent kind;
		float scale;
		float modifier;
		std::vector<NiAVObject*> nodes;
	};

	struct EmotionInfo {
		Actor* giantess;
		int ph_id;
		float speed_1;
		float speed_2;
		std::string_view task_name;
		std::string_view task_type;
	};

	struct VoreInformation {
		Actor* giantess;
		bool WasLiving;
		float Scale;
		float Vore_Power;
		float Health_Regeneration;
		float Box_Scale;
		float Duration;
		std::string_view Tiny_Name;
	};

	struct UnderFoot {
		Actor* giant;
		Actor* tiny;
		float force;
		/// Giant foot that is doing the squishing
		NiAVObject* foot;
		/// Tiny's body parts that are underfoot
		std::vector<NiAVObject*> bodyParts;
		FootEvent footEvent;
	};

	struct HighheelEquip {
		Actor* actor;
		bool equipping;
		float hhLength;
		NiPoint3 hhOffset;
		TESObjectARMO* shoe;
	};

	struct AddPerkEvent {
		Actor* actor;
		BGSPerk* perk;
		std::uint32_t rank;
	};

	struct RemovePerkEvent {
		Actor* actor;
		BGSPerk* perk;
	};

	class EventListener {
		public:
			EventListener() = default;
			~EventListener() = default;
			EventListener(EventListener const&) = delete;
			EventListener& operator=(EventListener const&) = delete;

			// Get name used for debug prints
			virtual std::string DebugName() = 0;

			// Called on Live (non paused) gameplay
			virtual void Update();

			virtual void BoneUpdate();

			// Called on Papyrus OnUpdate
			virtual void PapyrusUpdate();

			// Called on Havok update (when processing hitjobs)
			virtual void HavokUpdate();

			// Called when the camera update event is fired (in the TESCameraState)
			virtual void CameraUpdate();

			// Called on game load started (not yet finished)
			// and when new game is selected
			virtual void Reset();

			// Called when game is enabled (while not paused)
			virtual void Enabled();

			// Called when game is disabled (while not paused)
			virtual void Disabled();

			// Called when a game is started after a load/newgame
			virtual void Start();

			// Called when all forms are loaded (during game load before mainmenu)
			virtual void DataReady();

			// Called when an actor is reset
			virtual void ResetActor(Actor* actor);

			// Called when an actor has an item equipped
			virtual void ActorEquip(Actor* actor);

			// Called when Player absorbs dragon soul
			virtual void DragonSoulAbsorption();

			// Called when an actor has is fully loaded
			virtual void ActorLoaded(Actor* actor);

			// Called when a papyrus hit event is fired
			virtual void HitEvent(const TESHitEvent* evt);

			// Called when an actor is squashed underfoot
			virtual void UnderFootEvent(const UnderFoot& evt);

			// Fired when a foot lands
			virtual void OnImpact(const Impact& impact);

			// Fired when a highheel is (un)equiped or when an actor is loaded with HH
			virtual void OnHighheelEquip(const HighheelEquip& evt);

			// Fired when a perk is added
			virtual void OnAddPerk(const AddPerkEvent& evt);

			// Fired when a perk about to be removed
			virtual void OnRemovePerk(const RemovePerkEvent& evt);

			// Fired when a skyrim menu event occurs
			virtual void MenuChange(const MenuOpenCloseEvent* menu_event);

			// Fired when a actor animation event occurs
			virtual void ActorAnimEvent(Actor* actor, const std::string_view& tag, const std::string_view& payload);
	};

	class EventDispatcher {
		public:
			// EventDispatcher() = default;
			// ~EventDispatcher() = default;
			// EventDispatcher(EventDispatcher const&) = delete;
			// EventDispatcher& operator=(EventDispatcher const&) = delete;

			static void AddListener(EventListener* listener);
			static void DoUpdate();
			static void DoBoneUpdate();
			static void DoPapyrusUpdate();
			static void DoHavokUpdate();
			static void DoCameraUpdate();
			static void DoReset();
			static void DoEnabled();
			static void DoDisabled();
			static void DoStart();
			static void DoDataReady();
			static void DoResetActor(Actor* actor);
			static void DoActorEquip(Actor* actor);
			static void DoDragonSoulAbsorption();
			static void DoActorLoaded(Actor* actor);
			static void DoHitEvent(const TESHitEvent* evt);
			static void DoUnderFootEvent(const UnderFoot& evt);
			static void DoOnImpact(const Impact& impact);
			static void DoHighheelEquip(const HighheelEquip& impact);
			static void DoAddPerk(const AddPerkEvent& evt);
			static void DoRemovePerk(const RemovePerkEvent& evt);
			static void DoMenuChange(const MenuOpenCloseEvent* menu_event);
			static void DoActorAnimEvent(Actor* actor, const BSFixedString& a_tag, const BSFixedString& a_payload);
		private:
			[[nodiscard]] static EventDispatcher& GetSingleton();
			std::vector<EventListener*> listeners;
	};
}
