#pragma once

// Module that holds data that is persistent across saves

namespace GTS {

	//namespace Experiments {

		//static constexpr std::uint32_t fnv1a_32(const char* str, std::size_t length) {
		//	std::uint32_t hash = 0x811c9dc5;
		//	for (std::size_t i = 0; i < length; ++i) {
		//		hash ^= static_cast<std::uint32_t>(str[i]);
		//		hash *= 0x01000193;
		//	}
		//	return hash;
		//}

		template<typename T>
		bool CheckFloat(T& value) {
			if constexpr (std::is_floating_point_v<T>) {
				if (std::isnan(value)) {
					value = 0;
					return true;
				}
			}
			return false;
		}

		// Function to convert uint32_t to a string of 4 characters
		static std::string Uint32ToStr(uint32_t value) {
			char bytes[4];
			bytes[0] = static_cast<char>((value >> 24) & 0xFF);
			bytes[1] = static_cast<char>((value >> 16) & 0xFF);
			bytes[2] = static_cast<char>((value >> 8) & 0xFF);
			bytes[3] = static_cast<char>(value & 0xFF);
			return std::string(bytes, 4);
		}

		template <typename T, const uint32_t uid, const uint32_t ver = 1>
		struct BasicRecord {
			T value;
			static constexpr auto ID = std::byteswap(uid);

			BasicRecord() = default;
			BasicRecord(const T& val) : value(val) {}

			void Load(SKSE::SerializationInterface* serializationInterface, std::uint32_t type, std::uint32_t version, uint32_t size) {

				if (type == ID) {
					logger::trace("Cosave record {} is being read", Uint32ToStr(ID));
					if (version == ver && size == sizeof(T)) {
						if (serializationInterface->ReadRecordData(&value, sizeof(T))) {
							logger::trace("Cosave record {} was read", Uint32ToStr(ID));
							if (CheckFloat(value)) {
								logger::warn("Cosave record {} has been reset as it was of type float/double and was NaN!", Uint32ToStr(ID));
							}

							return;
						}
					}

					logger::error("Cosave record {} Could not be loaded", Uint32ToStr(ID));

				}
			}

			void Save(SKSE::SerializationInterface* serializationInterface) {
				logger::trace("Cosave record {} is being saved", Uint32ToStr(ID));
				if (serializationInterface->OpenRecord(ID, ver)) {
					if (serializationInterface->WriteRecordData(&value, sizeof(T))) {
						logger::trace("Cosave record {} save OK!", Uint32ToStr(ID));
						return;
					}
				}

				logger::error("Cosave record {} could not be saved", Uint32ToStr(ID));
			}

		};
	//}

	struct ActorData {

		float native_scale;
		float visual_scale;
		float visual_scale_v;
		float target_scale;
		float target_scale_v;
		float max_scale;
		float half_life;
		float anim_speed;
		float effective_multi; //<----- UNUSED
		float bonus_hp;        //<----- UNUSED
		float bonus_carry;     //<----- UNUSED
		float bonus_max_size;  //<----- UNUSED
		float smt_run_speed;   //<----- Useless here only player has it

		float NormalDamage;
		float SprintDamage;
		float FallDamage;
		float HHDamage;
		float SizeVulnerability; //<----- UNUSED

		float SizeReserve;

		float AllowHitGrowth;    //<----- Useless here only player and or follower use this,
								 //a npc specific toggle does not even exist

		float scaleOverride;     //WHY IS THE FP CAMERA SETTING STORED FOR EVERY SINGLE NPC!!!!

		float stolen_attributes;

		float stolen_health;
		float stolen_magick;
		float stolen_stamin;

		ActorData();
		explicit ActorData(Actor* actor);
	};

	struct CameraCollisions {
		bool enable_trees = true;
		bool enable_debris = true;
		bool enable_static = true;
		bool enable_terrain = true;
		bool enable_actor = false;
		float above_scale = 5.0f;
	};

	class Persistent : public EventListener {
		public:
			virtual std::string DebugName() override;
			virtual void Reset() override;
			virtual void ResetActor(Actor* actor) override;

			[[nodiscard]] static Persistent& GetSingleton() noexcept;
			static void OnRevert(SKSE::SerializationInterface*);
			static void OnGameSaved(SKSE::SerializationInterface* serde);
			static void OnGameLoaded(SKSE::SerializationInterface* serde);

			ActorData* GetActorData(Actor& actor);
			ActorData* GetActorData(Actor* actor);
			ActorData* GetData(TESObjectREFR* refr);
			ActorData* GetData(TESObjectREFR& refr);

			BasicRecord<int, 'TCST'> TrackedCameraState = 0;
			BasicRecord<bool, 'ECPL'> EnableCrawlPlayer = false;
			BasicRecord<bool, 'ECFL'> EnableCrawlFollower = false;

			bool vore_allowplayervore = false;
			bool AllowInsectVore = true;
			bool AllowUndeadVore = true;
			bool FollowerInteractions = false;
			bool vore_combatonly = true;
			bool allow_stagger = true;
			bool edit_voice_frequency = true;
			bool Vore_Ai = true;
			bool Stomp_Ai = true;
			bool Sandwich_Ai = true;
			bool Kick_Ai = true;
			bool Hugs_Ai = true;
			bool Butt_Ai = true;
			bool Thigh_Ai = true;
			bool delete_actors = true;
			bool hostile_toggle = true;
			bool legacy_sounds = false;
			bool actors_panic = true;
			bool launch_objects = true;
			bool NPCEffectImmunity = false;
			bool PCEffectImmunity = false;

			bool allow_weight_gain = true;

			float tremor_scale = 1.0f;
			float npc_tremor_scale = 1.0f;
			float size_related_damage_mult = 1.0f;
			float experience_mult = 1.0f;
			float progression_multiplier = 0.55f;

			//// ---- Quest Progression
			float HugStealCount = 0.0f; // Stage 1, Hugs
			float CrushCount = 0.0f; // Stage 2
			float STNCount = 0.0f; // Stage 3, Shrink to nothing
			float StolenSize = 0.0f; // Stage 4,
			float HandCrushed = 0.0f; // Stage 5, Crushed with hand
			float VoreCount = 0.0f; // Stage 6, Fully Vored
			float GiantCount = 0.0f; // Stage 7, Giant Count
			

			float stolen_attributes = 0.0f;

			float stolen_health = 0.0f;
			float stolen_magick = 0.0f;
			float stolen_stamin = 0.0f;

			float BalanceMode_SizeGain_Penalty = 1.0f;
			float BalanceMode_ShrinkRate_Base = 1.0f;
			float BalanceMode_ShrinkRate_Combat = 0.08f;

			SoftPotential MS_adjustment { //THIS SHOULD NOT BE HERE
				.k = 0.132f, //0.132
				.n = 0.86f,  //0.86
				.s = 1.12f,  //1.12
				.o = 1.0f,
				.a = 0.0f, //Default is 0
			};

			CameraCollisions camera_collisions;
		private:

			Persistent() = default;
			mutable std::mutex _lock;
			std::unordered_map<FormID, ActorData> _actor_data;
	};
}
