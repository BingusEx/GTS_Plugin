#include "Data/Transient.hpp"

namespace GTS {
	Transient& Transient::GetSingleton() noexcept {
		static Transient instance;
		return instance;
	}

	TempActorData* Transient::GetData(TESObjectREFR* object) {

		if (!object) {
			return nullptr;
		}

		auto key = object->formID;
		TempActorData* result = nullptr;

		try {
			result = &this->TempActorDataMap.at(key);
		}
		catch (const std::out_of_range& oor) {
			return nullptr;
		}

		return result;
	}

	TempActorData* Transient::GetActorData(Actor* actor) {

		std::unique_lock lock(this->TransientLock);

		if (!actor) {
			return nullptr;
		}

		auto key = actor->formID;

		try {
			return &this->TempActorDataMap.at(key);
		}
		catch (const std::out_of_range&) {

			// Try to add
			if (!actor) {
				return nullptr;
			}

			TempActorData NewTempActorData = {};

			auto bound_values = get_bound_values(actor);
			auto scale = get_scale(actor);

			if (scale < 0.0f) {
				log::info("Scale of {} is < 0", actor->GetDisplayFullName());
				return nullptr;
			}

			float base_height_unit = bound_values[2] * scale;
			float base_height_meters = unit_to_meter(base_height_unit);
			float fall_start = actor->GetPosition()[2];
			float last_set_fall_start = fall_start;
			float carryweight_boost = 0.0f;
			float health_boost = 0.0f;
			float SMT_Bonus_Duration = 0.0f;
			float SMT_Penalty_Duration = 0.0f;
			float FallTimer = 1.0f;
			float Hug_AnimSpeed = 1.0f;
			float Throw_Speed = 0.0f;

			float potion_max_size = 0.0f;
			float buttcrush_max_size = 0.0f;
			float buttcrush_start_scale = 0.0f;

			float SizeVulnerability = 0.0f;

			float push_force = 1.0f;
			
			bool Throw_WasThrown = false;

			bool can_do_vore = true;
			bool can_be_crushed = true;
			bool dragon_was_eaten = false;
			bool can_be_vored = true;
			bool being_held = false;
			bool is_between_breasts = false;
			bool about_to_be_eaten = false;
			bool being_foot_grinded = false;
			bool SMT_ReachedFullSpeed = false;
			bool OverrideCamera = false;
			bool WasReanimated = false;
			bool FPCrawling = false;
			bool FPProning = false;
			bool Overkilled = false;
			bool Protection = false;
			bool GrowthPotion = false;

			bool Devourment_Devoured = false;
			bool Devourment_Eaten = false;

			bool disable_collision = false;
			bool was_sneaking = false;

			bool emotion_modifier_busy = false;
			bool emotion_phenome_busy = false;

			float IsNotImmune = 1.0f;

			NiPoint3 POS_Last_Leg_L = NiPoint3(0.0f, 0.0f, 0.0f);
			NiPoint3 POS_Last_Leg_R = NiPoint3(0.0f, 0.0f, 0.0f);
			NiPoint3 POS_Last_Hand_L = NiPoint3(0.0f, 0.0f, 0.0f);
			NiPoint3 POS_Last_Hand_R = NiPoint3(0.0f, 0.0f, 0.0f);

			float shrink_until = 0.0f;

			Actor* IsInControl = nullptr;

			std::vector<Actor*> shrinkies = {};

			TESObjectREFR* disable_collision_with = nullptr;
			TESObjectREFR* Throw_Offender = nullptr;

			AttachToNode AttachmentNode = AttachToNode::None;
			BusyFoot FootInUse = BusyFoot::None;
			
			float otherScales = 1.0f;
			float vore_recorded_scale = 1.0f;
			float WorldFov_Default = 0;
			float FpFov_Default = 0;
			float ButtCrushGrowthAmount = 0;
			float MovementSlowdown = 1.0f;
			float ShrinkResistance = 0.0f;
			float MightValue = 0.0f;
			float Shrink_Ticks = 0.0f;
			float Shrink_Ticks_Calamity = 0.0f;
			

			float Perk_BonusActionSpeed = 1.0f;
			float Perk_lifeForceStolen = 0.0f;
			int Perk_lifeForceStacks = 0;

			int CrushedTinies = 0;

			NiPoint3 BoundingBox_Cache = get_bound_values(actor); // Default Human Height

			// Volume scales cubically
			float base_volume = bound_values[0] * bound_values[1] * bound_values[2] * scale * scale * scale;
			float base_volume_meters = unit_to_meter(base_volume);

			constexpr float rip_initScale = -1.0f;
			float breast_size_buff = 0.0f;

			NewTempActorData.base_height = base_height_meters;
			NewTempActorData.base_volume = base_volume_meters;

			auto shoe = actor->GetWornArmor(BGSBipedObjectForm::BipedObjectSlot::kFeet);

			float shoe_weight = 1.0f;

			if (shoe) {
				shoe_weight = shoe->weight;
			}

			NewTempActorData.shoe_weight = shoe_weight;
			NewTempActorData.char_weight = actor->GetWeight();
			NewTempActorData.fall_start = fall_start;
			NewTempActorData.last_set_fall_start = last_set_fall_start;
			NewTempActorData.carryweight_boost = carryweight_boost;
			NewTempActorData.health_boost = health_boost;
			NewTempActorData.SMT_Bonus_Duration = SMT_Bonus_Duration;
			NewTempActorData.SMT_Penalty_Duration = SMT_Penalty_Duration;
			NewTempActorData.FallTimer = FallTimer;
			NewTempActorData.Hug_AnimSpeed = Hug_AnimSpeed;
			NewTempActorData.Throw_Speed = Throw_Speed;
			NewTempActorData.potion_max_size = potion_max_size;
			NewTempActorData.buttcrush_max_size = buttcrush_max_size;
			NewTempActorData.buttcrush_start_scale = buttcrush_start_scale;
			NewTempActorData.SizeVulnerability = SizeVulnerability;

			NewTempActorData.push_force = push_force;
			NewTempActorData.can_do_vore = can_do_vore;
			NewTempActorData.Throw_WasThrown = Throw_WasThrown;
			NewTempActorData.can_be_crushed = can_be_crushed;
			NewTempActorData.being_held = being_held;
			NewTempActorData.is_between_breasts = is_between_breasts;
			NewTempActorData.about_to_be_eaten = about_to_be_eaten;
			NewTempActorData.being_foot_grinded = being_foot_grinded;
			NewTempActorData.SMT_ReachedFullSpeed = SMT_ReachedFullSpeed;
			NewTempActorData.dragon_was_eaten = dragon_was_eaten;
			NewTempActorData.can_be_vored = can_be_vored;
			NewTempActorData.disable_collision_with = disable_collision_with;
			NewTempActorData.Throw_Offender = Throw_Offender;
			NewTempActorData.AttachmentNode = AttachmentNode;
			NewTempActorData.FootInUse = FootInUse;
			NewTempActorData.otherScales = otherScales;
			NewTempActorData.vore_recorded_scale = vore_recorded_scale;
			NewTempActorData.WorldFov_Default = WorldFov_Default;
			NewTempActorData.FpFov_Default = FpFov_Default;
			NewTempActorData.ButtCrushGrowthAmount = ButtCrushGrowthAmount;
			NewTempActorData.MovementSlowdown = MovementSlowdown;
			NewTempActorData.ShrinkResistance = ShrinkResistance;
			NewTempActorData.MightValue = MightValue;
			NewTempActorData.Shrink_Ticks = Shrink_Ticks;
			NewTempActorData.Shrink_Ticks_Calamity = Shrink_Ticks_Calamity;

			NewTempActorData.Perk_BonusActionSpeed = Perk_BonusActionSpeed;
			NewTempActorData.Perk_lifeForceStolen = Perk_lifeForceStolen;
			NewTempActorData.Perk_lifeForceStacks = Perk_lifeForceStacks;

			NewTempActorData.CrushedTinies = CrushedTinies;

			NewTempActorData.BoundingBox_Cache = BoundingBox_Cache;

			NewTempActorData.OverrideCamera = OverrideCamera;
			NewTempActorData.WasReanimated = WasReanimated;
			NewTempActorData.FPCrawling = FPCrawling;
			NewTempActorData.FPProning = FPProning;
			NewTempActorData.Overkilled = Overkilled;
			NewTempActorData.Protection = Protection;
			NewTempActorData.GrowthPotion = GrowthPotion;

			NewTempActorData.Devourment_Devoured = Devourment_Devoured;
			NewTempActorData.Devourment_Eaten = Devourment_Eaten;

			NewTempActorData.disable_collision = disable_collision;
			NewTempActorData.was_sneaking = was_sneaking;

			NewTempActorData.emotion_modifier_busy = emotion_modifier_busy;
			NewTempActorData.emotion_phenome_busy = emotion_phenome_busy;

			NewTempActorData.IsNotImmune = IsNotImmune;

			NewTempActorData.POS_Last_Leg_L = POS_Last_Leg_L;
			NewTempActorData.POS_Last_Leg_R = POS_Last_Leg_R;
			NewTempActorData.POS_Last_Hand_L = POS_Last_Hand_L;
			NewTempActorData.POS_Last_Hand_R = POS_Last_Hand_R;

			NewTempActorData.shrinkies = shrinkies;
			NewTempActorData.shrink_until = shrink_until;

			NewTempActorData.IsInControl = IsInControl;
		
			NewTempActorData.rip_lastScale = rip_initScale;
			NewTempActorData.rip_offset = rip_initScale;

			NewTempActorData.breast_size_buff = breast_size_buff;
			NewTempActorData.GameModeIntervalTimer = Timer(0);

			this->TempActorDataMap.try_emplace(key, NewTempActorData);

			return &this->TempActorDataMap.at(key);
		}
	}

	std::vector<FormID> Transient::GetForms() const {
		std::vector<FormID> keys;
		keys.reserve(this->TempActorDataMap.size());
		for(const auto data : this->TempActorDataMap | views::keys) {
			keys.push_back(data);
		}
		return keys;
	}


	std::string Transient::DebugName() {
		return "Transient";
	}

	void Transient::Update() {

		for (Actor* actor : find_actors()) {

			if (!actor) {
				continue;
			}

			if (!actor->Is3DLoaded()) {
				continue;
			}

			FormID ActorKey = actor->formID;
			std::unique_lock lock(this->TransientLock);

			try {
				auto data = this->TempActorDataMap.at(ActorKey);
				data.char_weight = actor->GetWeight();
			}
			catch (...) {
				
			}
		}
	}

	void Transient::Reset() {
		log::info("Transient was reset");
		std::unique_lock lock(this->TransientLock);
		this->TempActorDataMap.clear();
	}

	void Transient::ResetActor(Actor* actor) {
		std::unique_lock lock(this->TransientLock);
		if (actor) {
			auto key = actor->formID;
			this->TempActorDataMap.erase(key);
		}
	}
}
