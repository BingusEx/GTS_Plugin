
#include "Data/Persistent.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Utils/ItemDistributor.hpp"

namespace {


	inline const auto ActorDataRecord = _byteswap_ulong('ACTD');

	inline const auto AllowPlayerVoreRecord = _byteswap_ulong('APVR');
	inline const auto AllowFollowerInteractions = _byteswap_ulong('AVFI');
	inline const auto AllowStaggerRecord = _byteswap_ulong('ASRD');
	inline const auto EditVoiceFrequency = _byteswap_ulong('EVFQ');
	inline const auto VoreCombatOnlyRecord = _byteswap_ulong('VRCO');
	inline const auto TremorScales = _byteswap_ulong('TREM');
	inline const auto CamCollisions = _byteswap_ulong('CAMC');
	inline const auto SizeDamageMult = _byteswap_ulong('SZDM');
	inline const auto XpMult = _byteswap_ulong('XPMT');
	inline const auto StompAiRecord = _byteswap_ulong('STAI');
	inline const auto SandwichAiRecord = _byteswap_ulong('SWAI');
	inline const auto KickAiRecord = _byteswap_ulong('KKAI');
	inline const auto HugsAiRecord = _byteswap_ulong('HSAI');
	inline const auto ThighAiRecord = _byteswap_ulong('THAI');
	inline const auto ButtAiRecord = _byteswap_ulong('BTAI');
	inline const auto VoreAiRecord = _byteswap_ulong('VRAI');
	inline const auto ProgressionMult = _byteswap_ulong('PRMT');
	inline const auto DeleteActors = _byteswap_ulong('DTAS');
	inline const auto HostileToggle = _byteswap_ulong('HTTL');
	inline const auto LegacySounds = _byteswap_ulong('LGSD');
	inline const auto ActorsPanic = _byteswap_ulong('ACTP');
	inline const auto NPC_EffectImmunity = _byteswap_ulong('NPER');
	inline const auto PC_EffectImmunity = _byteswap_ulong('PCER');
	inline const auto Balance_SGP = _byteswap_ulong('BMSP');
	inline const auto Balance_SRB = _byteswap_ulong('BMSB');
	inline const auto Balance_SRC = _byteswap_ulong('BMSC');

	inline const auto StolenAttributes = _byteswap_ulong('STAT');
	inline const auto Att_HealthStorage = _byteswap_ulong('HTSG');
	inline const auto Att_StaminStorage = _byteswap_ulong('STSG');
	inline const auto Att_MagickStorage = _byteswap_ulong('MTSG');

	// Quest
	inline const auto Record_StolenSize = _byteswap_ulong('QSSR');
	inline const auto Record_CrushCount = _byteswap_ulong('QCCR');
	inline const auto Record_STNCount = _byteswap_ulong('QSTR');
	inline const auto Record_HugStealCount = _byteswap_ulong('QHSR');
	inline const auto Record_HandCrushed = _byteswap_ulong('QHCR');
	inline const auto Record_VoreCount = _byteswap_ulong('QVRR');
	inline const auto Record_GiantCount = _byteswap_ulong('QGCR');
	//

	constexpr float DEFAULT_MAX_SCALE = 65535.0f;
	constexpr float DEFAULT_HALF_LIFE = 1.0f;
}

namespace GTS {


	Persistent& Persistent::GetSingleton() noexcept {
		static Persistent instance;
		return instance;
	}

	std::string Persistent::DebugName() {
		return "Persistent";
	}

	void Persistent::Reset() {
		//Plugin::SetInGame(false);
		std::unique_lock lock(this->_lock);
		this->ActorDataMap.clear();

	    // Ensure we reset them back to inital scales
	    // if they are loaded into game memory
	    // since skyrim only lazy loads actors
	    // that are already in memory it won't reload
	    // their nif scales otherwise
	    for (auto actor: find_actors()) {
			ResetToInitScale(actor);
		}
	}

	void Persistent::OnRevert(SerializationInterface*) {
		GetSingleton().Reset();
	}



	void Persistent::OnGameLoaded(SerializationInterface* serde) {

		std::uint32_t type;
		std::uint32_t size;
		std::uint32_t version;

		SizeManager::GetSingleton().Reset();
		//CooldownManager::GetSingleton().Reset();

		DistributeChestItems();

		FixAnimationsAndCamera(); // Call it from ActorUtils, needed to fix Grab anim on save-reload


		auto& Persi = Persistent::GetSingleton();

		while (serde->GetNextRecordInfo(type, version, size)) {

			Persi.TrackedCameraState.Load(serde, type, version, size);
			Persi.EnableCrawlPlayer.Load(serde, type, version, size);
			Persi.EnableCrawlFollower.Load(serde, type, version, size);

			//Dummy Float Used To Advance Actor Data Reads.
			float PAD_DUMMY = 0.0;

			if (type == ActorDataRecord) {

				if (version >= 1) {

					std::size_t RecordCount = 0;
					serde->ReadRecordData(&RecordCount, sizeof(RecordCount));

					for (; RecordCount > 0; --RecordCount) {

						// --------- VERSION 1 OF STRUCT DATA BEGINS HERE
						// --------- ACTOR FORMID

						RE::FormID ReadFormID;        //FormID Stored in the Cosave;
						RE::FormID NewActorFormID;    //Loadorder may have changed. This is the New FormID
						serde->ReadRecordData(&ReadFormID, sizeof(ReadFormID));
						if (!serde->ResolveFormID(ReadFormID, NewActorFormID)) {
							log::warn("Actor FormID {:X} Not found in CoSave. Skipping...", ReadFormID);
							continue;
						}

						// --------- NATIVE_SCALE

						float native_scale;
						serde->ReadRecordData(&native_scale, sizeof(native_scale));
						if (std::isnan(native_scale)) {
							native_scale = 1.0f;
						}

						// --------- VISUAL_SCALE

						float visual_scale;
						serde->ReadRecordData(&visual_scale, sizeof(visual_scale));
						if (std::isnan(visual_scale)) {
							visual_scale = 1.0f;
						}

						// --------- VISUAL_SCALE_V

						float visual_scale_v;
						serde->ReadRecordData(&visual_scale_v, sizeof(visual_scale_v));
						if (std::isnan(visual_scale_v)) {
							visual_scale_v = 0.0f;
						}

						// --------- TARGET_SCALE

						float target_scale;
						serde->ReadRecordData(&target_scale, sizeof(target_scale));
						if (std::isnan(target_scale)) {
							target_scale = 1.0f;
						}

						// --------- MAX_SCALE

						float max_scale;
						serde->ReadRecordData(&max_scale, sizeof(max_scale));
						if (std::isnan(max_scale)) {
							max_scale = DEFAULT_MAX_SCALE;
						}

						// --------- VERSION 2 OF STRUCT DATA BEGINS HERE
						// --------- HALF_LIFE

						float half_life;
						if (version >= 2) {
							serde->ReadRecordData(&half_life, sizeof(half_life));
						} else {
							half_life = DEFAULT_HALF_LIFE;
						}
						if (std::isnan(half_life)) {
							half_life = DEFAULT_HALF_LIFE;
						}

						// --------- VERSION 3 OF STRUCT DATA BEGINS HERE
						// --------- ANIM_SPEED

						float anim_speed;
						if (version >= 3) {
							serde->ReadRecordData(&anim_speed, sizeof(anim_speed));
						} else {
							anim_speed = 1.0f;
						}
						if (std::isnan(anim_speed)) {
							anim_speed = 1.0f;
						}

						// --------- VERSION 4 OF STRUCT DATA BEGINS HERE
						// --------- EFFECTIVE_MULTI

						float effective_multi;
						if (version >= 4) {
							serde->ReadRecordData(&effective_multi, sizeof(effective_multi));
						} else {
							effective_multi = 1.0f;
						}
						if (std::isnan(effective_multi)) {
							effective_multi = 1.0f;
						}

						// --------- VERSION 5 OF STRUCT DATA BEGINS HERE
						// --------- BONUS_HP

						float bonus_hp;
						if (version >= 5) {
							serde->ReadRecordData(&bonus_hp, sizeof(bonus_hp));
						} else {
							bonus_hp = 0.0f;
						}
						if (std::isnan(bonus_hp)) {
							bonus_hp = 0.0f;
						}

						// --------- BONUS_CARRY

						float bonus_carry;
						if (version >= 5) {
							serde->ReadRecordData(&bonus_carry, sizeof(bonus_carry));
						} else {
							bonus_carry = 0.0f;
						}
						if (std::isnan(bonus_carry)) {
							bonus_carry = 0.0f;
						}

						// --------- BONUS_MAX_SIZE

						float bonus_max_size;
						if (version >= 5) {
							serde->ReadRecordData(&bonus_max_size, sizeof(bonus_max_size));
						} else {
							bonus_max_size = 0.0f;
						}
						if (std::isnan(bonus_max_size)) {
							bonus_max_size = 0.0f;
						}

						// --------- VERSION 6 OF STRUCT DATA BEGINS HERE
						// --------- SMT_RUN_SPEED

						float smt_run_speed;
						if (version >= 6) {
							serde->ReadRecordData(&smt_run_speed, sizeof(smt_run_speed));
						} else {
							smt_run_speed = 0.0f;
						}
						if (std::isnan(smt_run_speed)) {
							smt_run_speed = 0.0f;
						}

						// --------- NORMALDAMAGE

						float NormalDamage; //0
						if (version >= 6) {
							serde->ReadRecordData(&NormalDamage, sizeof(NormalDamage));
						} else {
							NormalDamage = 0.0f;
						}
						if (std::isnan(NormalDamage)) {
							NormalDamage = 0.0f;
						}

						// --------- SPRINTDAMAGE

						float SprintDamage; //1
						if (version >= 6) {
							serde->ReadRecordData(&SprintDamage, sizeof(SprintDamage));
						} else {
							SprintDamage = 0.0f;
						}
						if (std::isnan(SprintDamage)) {
							SprintDamage = 0.0f;
						}

						// --------- FALLDAMAGE

						float FallDamage; //2
						if (version >= 6) {
							serde->ReadRecordData(&FallDamage, sizeof(FallDamage));
						} else {
							FallDamage = 0.0f;
						}
						if (std::isnan(FallDamage)) {
							FallDamage = 0.0f;
						}

						// --------- HHDAMAGE

						float HHDamage; //3
						if (version >= 6) {
							serde->ReadRecordData(&HHDamage, sizeof(HHDamage));
						} else {
							HHDamage = 0.0f;
						}
						if (std::isnan(HHDamage)) {
							HHDamage = 0.0f;
						}

						// --------- SIZEINVUNERABILITY

						float SizeVulnerability;
						if (version >= 6) {
							serde->ReadRecordData(&SizeVulnerability, sizeof(SizeVulnerability));
						} else {
							SizeVulnerability = 0.0f;
						}
						if (std::isnan(SizeVulnerability)) {
							SizeVulnerability = 0.0f;
						}

						// --------- UNUSED @ 0x48 OFFSET

						if (version >= 6) {
							serde->ReadRecordData(&PAD_DUMMY, sizeof(PAD_DUMMY));
						}

						// --------- SIZERESERVE

						float SizeReserve;
						if (version >= 6) {
							serde->ReadRecordData(&SizeReserve, sizeof(SizeReserve));
						} else {
							SizeReserve = 0.0f;
						}
						if (std::isnan(SizeReserve)) {
							SizeReserve = 0.0f;
						}

						// --------- VERSION 7 OF STRUCT DATA BEGINS HERE
						// --------- TARGET_SCALE_V

						float target_scale_v;
						if (version >= 7) {
							serde->ReadRecordData(&target_scale_v, sizeof(target_scale_v));
						} else {
							target_scale_v = 0.0f;
						}
						if (std::isnan(target_scale_v)) {
							target_scale_v = 0.0f;
						}

						// --------- VERSION 8 OF STRUCT DATA BEGINS HERE
						// --------- SCALEOVVERIDE

						float scaleOverride;
						if (version >= 8) {
							serde->ReadRecordData(&scaleOverride, sizeof(scaleOverride));
						} else {
							scaleOverride = -1.0f;
						}
						if (std::isnan(scaleOverride)) {
							scaleOverride = -1.0f;
						}

						// --------- STOLLEN_ATTRIBUTES

						float stolen_attributes;
						if (version >= 8) {
							serde->ReadRecordData(&stolen_attributes, sizeof(stolen_attributes));
						} else {
							stolen_attributes = 0.0f;
						}
						if (std::isnan(stolen_attributes)) {
							stolen_attributes = 0.0f;
						}

						// --------- STOLEN_HEALTH

						float stolen_health;
						if (version >= 8) {
							serde->ReadRecordData(&stolen_health, sizeof(stolen_health));
						} else {
							stolen_health = 0.0f;
						}
						if (std::isnan(stolen_health)) {
							stolen_health = 0.0f;
						}

						// --------- STOLEN_MAGICKA

						float stolen_magick;
						if (version >= 8) {
							serde->ReadRecordData(&stolen_magick, sizeof(stolen_magick));
						} else {
							stolen_magick = 0.0f;
						}
						if (std::isnan(stolen_magick)) {
							stolen_magick = 0.0f;
						}

						// --------- STOLEN_STAMINA

						float stolen_stamin;
						if (version >= 8) {
							serde->ReadRecordData(&stolen_stamin, sizeof(stolen_stamin));
						} else {
							stolen_stamin = 0.0f;
						}
						if (std::isnan(stolen_stamin)) {
							stolen_stamin = 0.0f;
						}

						// ----------- END OF COSAVE READS
						// ----------------------------------------------------------------------------------------------------------------------

						ActorData data = ActorData();

						//log::info("Loading Actor {:X} with data, native_scale: {}, visual_scale: {}, visual_scale_v: {}, target_scale: {}, max_scale: {}, half_life: {}, anim_speed: {}, bonus_hp: {}, bonus_carry: {}", newActorFormID, native_scale, visual_scale, visual_scale_v, target_scale, max_scale, half_life, anim_speed, bonus_hp, bonus_carry);

						data.native_scale = native_scale;
						data.visual_scale = visual_scale;
						data.visual_scale_v = visual_scale_v;
						data.target_scale = target_scale;
						data.max_scale = max_scale;
						data.half_life = half_life;
						data.anim_speed = anim_speed;
						data.effective_multi = effective_multi;
						data.bonus_hp = bonus_hp;
						data.bonus_carry = bonus_carry;
						data.bonus_max_size = bonus_max_size;
						data.smt_run_speed = smt_run_speed;
						data.NormalDamage = NormalDamage;
						data.SprintDamage = SprintDamage;
						data.FallDamage = FallDamage;
						data.HHDamage = HHDamage;
						data.SizeVulnerability = SizeVulnerability;
						data.SizeReserve = SizeReserve;
						data.target_scale_v = target_scale_v;
						data.scaleOverride = scaleOverride;
						data.stolen_attributes = stolen_attributes;
						data.stolen_health = stolen_health;
						data.stolen_magick = stolen_magick;
						data.stolen_stamin = stolen_stamin;


						TESForm* actor_form = TESForm::LookupByID<Actor>(NewActorFormID);

						if (actor_form) {

							Actor* actor = skyrim_cast<Actor*>(actor_form);

							if (actor) {
								Persistent::GetSingleton().ActorDataMap.insert_or_assign(NewActorFormID, data);
							}
							else {
								log::warn("Actor ID {:X} could not be found after loading the save.", NewActorFormID);
							}
						}
						else {
							log::warn("Actor ID {:X} could not be found after loading the save.", NewActorFormID);
						}
					}
				}
				else {
					log::info("Disregarding version 0 cosave info.");
				}
			}

			else if (type == AllowPlayerVoreRecord) {
				bool vore_allowplayervore;
				serde->ReadRecordData(&vore_allowplayervore, sizeof(vore_allowplayervore));
				GetSingleton().vore_allowplayervore = vore_allowplayervore;
			}

			else if (type == AllowFollowerInteractions) {
				bool FollowerInteractions;
				serde->ReadRecordData(&FollowerInteractions, sizeof(FollowerInteractions));
				GetSingleton().FollowerInteractions = FollowerInteractions;
			}
			else if (type == VoreCombatOnlyRecord) {
				bool vore_combatonly;
				serde->ReadRecordData(&vore_combatonly, sizeof(vore_combatonly));
				GetSingleton().vore_combatonly = vore_combatonly;
			}
			else if (type == AllowStaggerRecord) {
				bool allow_stagger;
				serde->ReadRecordData(&allow_stagger, sizeof(allow_stagger));
				GetSingleton().allow_stagger = allow_stagger;
			}
			else if (type == EditVoiceFrequency) {
				bool edit_voice_frequency;
				serde->ReadRecordData(&edit_voice_frequency, sizeof(edit_voice_frequency));
				GetSingleton().edit_voice_frequency = edit_voice_frequency;
			}
			else if (type == StompAiRecord) {
				bool Stomp_Ai;
				serde->ReadRecordData(&Stomp_Ai, sizeof(Stomp_Ai));
				GetSingleton().Stomp_Ai = Stomp_Ai;
			}
			else if (type == DeleteActors) {
				bool delete_actors;
				serde->ReadRecordData(&delete_actors, sizeof(delete_actors));
				GetSingleton().delete_actors = delete_actors;
			}
			else if (type == LegacySounds) {
				bool legacy_sounds;
				serde->ReadRecordData(&legacy_sounds, sizeof(legacy_sounds));
				GetSingleton().legacy_sounds = legacy_sounds;
			}
			else if (type == ActorsPanic) {
				bool actors_panic;
				serde->ReadRecordData(&actors_panic, sizeof(actors_panic));
				GetSingleton().actors_panic = actors_panic;
			}
			else if (type == StolenAttributes) {
				float stolen_attributes;
				serde->ReadRecordData(&stolen_attributes, sizeof(stolen_attributes));
				GetSingleton().stolen_attributes = stolen_attributes;
			}
			else if (type == Att_HealthStorage) {
				float stolen_health;
				serde->ReadRecordData(&stolen_health, sizeof(stolen_health));
				GetSingleton().stolen_health = stolen_health;
			}
			else if (type == Att_MagickStorage) {
				float stolen_magick;
				serde->ReadRecordData(&stolen_magick, sizeof(stolen_magick));
				GetSingleton().stolen_magick = stolen_magick;
			}
			else if (type == Att_StaminStorage) {
				float stolen_stamin;
				serde->ReadRecordData(&stolen_stamin, sizeof(stolen_stamin));
				GetSingleton().stolen_stamin = stolen_stamin;
			}
			/////////////////////////////////////////////////////////////////////////// Quest
			else if (type == Record_StolenSize) { // stage 1
				float StolenSize;
				serde->ReadRecordData(&StolenSize, sizeof(StolenSize));
				GetSingleton().StolenSize = StolenSize;
			}
			else if (type == Record_CrushCount) { // stage 2
				float CrushCount;
				serde->ReadRecordData(&CrushCount, sizeof(CrushCount));
				GetSingleton().CrushCount = CrushCount;
			}
			else if (type == Record_STNCount) { // stage 3
				float STNCount;
				serde->ReadRecordData(&STNCount, sizeof(STNCount));
				GetSingleton().STNCount = STNCount;
			}
			else if (type == Record_HugStealCount) { // stage 4
				float HugStealCount;
				serde->ReadRecordData(&HugStealCount, sizeof(HugStealCount));
				GetSingleton().HugStealCount = HugStealCount;
			}
			else if (type == Record_HandCrushed) { // stage 5
				float HandCrushed;
				serde->ReadRecordData(&HandCrushed, sizeof(HandCrushed));
				GetSingleton().HandCrushed = HandCrushed;
			}
			else if (type == Record_VoreCount) { // stage 6
				float VoreCount;
				serde->ReadRecordData(&VoreCount, sizeof(VoreCount));
				GetSingleton().VoreCount = VoreCount;
			}
			else if (type == Record_GiantCount) { // stage 7
				float GiantCount;
				serde->ReadRecordData(&GiantCount, sizeof(GiantCount));
				GetSingleton().GiantCount = GiantCount;
			}
			///////////////////////////////////////////////////////////////////////////
			else if (type == HostileToggle) {
				bool hostile_toggle;
				serde->ReadRecordData(&hostile_toggle, sizeof(hostile_toggle));
				GetSingleton().hostile_toggle = hostile_toggle;
			}
			else if (type == SandwichAiRecord) {
				bool Sandwich_Ai;
				serde->ReadRecordData(&Sandwich_Ai, sizeof(Sandwich_Ai));
				GetSingleton().Sandwich_Ai = Sandwich_Ai;
			}
			else if (type == KickAiRecord) {
				bool Kick_Ai;
				serde->ReadRecordData(&Kick_Ai, sizeof(Kick_Ai));
				GetSingleton().Kick_Ai = Kick_Ai;
			}
			else if (type == ButtAiRecord) {
				bool Butt_Ai;
				serde->ReadRecordData(&Butt_Ai, sizeof(Butt_Ai));
				GetSingleton().Butt_Ai = Butt_Ai;
			}
			else if (type == HugsAiRecord) {
				bool Hugs_Ai;
				serde->ReadRecordData(&Hugs_Ai, sizeof(Hugs_Ai));
				GetSingleton().Hugs_Ai = Hugs_Ai;
			}
			else if (type == ThighAiRecord) {
				bool Thigh_Ai;
				serde->ReadRecordData(&Thigh_Ai, sizeof(Thigh_Ai));
				GetSingleton().Thigh_Ai = Thigh_Ai;
			}
			else if (type == VoreAiRecord) {
				bool Vore_Ai;
				serde->ReadRecordData(&Vore_Ai, sizeof(Vore_Ai));
				GetSingleton().Vore_Ai = Vore_Ai;
			}
			else if (type == Balance_SGP) {
				float BalanceMode_SizeGain_Penalty;
				serde->ReadRecordData(&BalanceMode_SizeGain_Penalty, sizeof(BalanceMode_SizeGain_Penalty));
				GetSingleton().BalanceMode_SizeGain_Penalty = BalanceMode_SizeGain_Penalty;
			}
			else if (type == Balance_SRB) {
				float BalanceMode_ShrinkRate_Base;
				serde->ReadRecordData(&BalanceMode_ShrinkRate_Base, sizeof(BalanceMode_ShrinkRate_Base));
				GetSingleton().BalanceMode_ShrinkRate_Base = BalanceMode_ShrinkRate_Base;
			}
			else if (type == Balance_SRC) {
				float BalanceMode_ShrinkRate_Combat;
				serde->ReadRecordData(&BalanceMode_ShrinkRate_Combat, sizeof(BalanceMode_ShrinkRate_Combat));
				GetSingleton().BalanceMode_ShrinkRate_Combat = BalanceMode_ShrinkRate_Combat;
			}
			else if (type == NPC_EffectImmunity) {
				bool NPCEffectImmunity;
				serde->ReadRecordData(&NPCEffectImmunity, sizeof(NPCEffectImmunity));
				GetSingleton().NPCEffectImmunity = NPCEffectImmunity;
			}
			else if (type == PC_EffectImmunity) {
				bool PCEffectImmunity;
				serde->ReadRecordData(&PCEffectImmunity, sizeof(PCEffectImmunity));
				GetSingleton().PCEffectImmunity = PCEffectImmunity;
			}
			else if (type == ProgressionMult) {
				float progression_multiplier;
				serde->ReadRecordData(&progression_multiplier, sizeof(progression_multiplier));
				GetSingleton().progression_multiplier = progression_multiplier;
			}
			else if (type == SizeDamageMult) {
				float size_related_damage_mult;
				serde->ReadRecordData(&size_related_damage_mult, sizeof(size_related_damage_mult));
				GetSingleton().size_related_damage_mult = size_related_damage_mult;
			}
			else if (type == XpMult) {
				float experience_mult;
				serde->ReadRecordData(&experience_mult, sizeof(experience_mult));
				GetSingleton().experience_mult = experience_mult;
			}
			else if (type == TremorScales) {

				///UNUSED REMOVE THIS VALUE
				float tremor_scale;
				serde->ReadRecordData(&tremor_scale, sizeof(tremor_scale));
				GetSingleton().tremor_scale = tremor_scale;
				float npc_tremor_scale;
				serde->ReadRecordData(&npc_tremor_scale, sizeof(npc_tremor_scale));
				GetSingleton().npc_tremor_scale = npc_tremor_scale;
			}
			else if (type == CamCollisions) {
				bool enable_trees;
				serde->ReadRecordData(&enable_trees, sizeof(enable_trees));
				GetSingleton().camera_collisions.enable_trees = enable_trees;
				bool enable_debris;
				serde->ReadRecordData(&enable_debris, sizeof(enable_debris));
				GetSingleton().camera_collisions.enable_debris = enable_debris;
				bool enable_terrain;
				serde->ReadRecordData(&enable_terrain, sizeof(enable_terrain));
				GetSingleton().camera_collisions.enable_terrain = enable_terrain;
				bool enable_actor;
				serde->ReadRecordData(&enable_actor, sizeof(enable_actor));
				GetSingleton().camera_collisions.enable_actor = enable_actor;
				if (version >= 1) {
					bool enable_static;
					serde->ReadRecordData(&enable_static, sizeof(enable_static));
					GetSingleton().camera_collisions.enable_static = enable_static;
				}
				float above_scale;
				serde->ReadRecordData(&above_scale, sizeof(above_scale));
				GetSingleton().camera_collisions.above_scale = above_scale;
			}
			else {
				log::warn("Unknown record type in cosave.");
			}
		}
	}

	void Persistent::OnGameSaved(SerializationInterface* serde) {
		std::unique_lock lock(GetSingleton()._lock);

		if (!serde->OpenRecord(ActorDataRecord, 8)) {
			log::error("Unable to open actor data record to write cosave data.");
			return;
		}


		auto& Persi = Persistent::GetSingleton();
		auto count = GetSingleton().ActorDataMap.size();

		serde->WriteRecordData(&count, sizeof(count));

		const float DUMMY_FLOAT = 0.0f;

		for (auto const& [form_id_t, data] : GetSingleton().ActorDataMap) {

			FormID form_id = form_id_t;
			float native_scale = data.native_scale;
			float visual_scale = data.visual_scale;
			float visual_scale_v = data.visual_scale_v;
			float target_scale = data.target_scale;
			float max_scale = data.max_scale;
			float half_life = data.half_life;
			float anim_speed = data.anim_speed;
			float effective_multi = data.effective_multi;
			float bonus_hp = data.bonus_hp;
			float bonus_carry = data.bonus_carry;
			float bonus_max_size = data.bonus_max_size;
			float smt_run_speed = data.smt_run_speed;
			float NormalDamage = data.NormalDamage;
			float SprintDamage = data.SprintDamage;
			float FallDamage = data.FallDamage;
			float HHDamage = data.HHDamage;
			float SizeVulnerability = data.SizeVulnerability;
			//data.PAD_44;
			float SizeReserve = data.SizeReserve;
			float target_scale_v = data.target_scale_v;
			float scaleOverride = data.scaleOverride;

			float stolen_attributes = data.stolen_attributes;
			float stolen_health = data.stolen_health;
			float stolen_magick = data.stolen_magick;
			float stolen_stamin = data.stolen_stamin;

			//log::info("Saving Actor {:X} with data, native_scale: {}, visual_scale: {}, visual_scale_v: {}, target_scale: {}, max_scale: {}, half_life: {}, anim_speed: {}, effective_multi: {}, effective_multi: {}, bonus_hp: {}, bonus_carry: {}, bonus_max_size: {}", form_id, native_scale, visual_scale, visual_scale_v, target_scale, max_scale, half_life, anim_speed, effective_multi, effective_multi, bonus_hp, bonus_carry, bonus_max_size);
			serde->WriteRecordData(&form_id, sizeof(form_id));                    //0x00
			serde->WriteRecordData(&native_scale, sizeof(native_scale));          //0x04
			serde->WriteRecordData(&visual_scale, sizeof(visual_scale));          //0x08
			serde->WriteRecordData(&visual_scale_v, sizeof(visual_scale_v));      //0x0C
			serde->WriteRecordData(&target_scale, sizeof(target_scale));          //0x10
			serde->WriteRecordData(&max_scale, sizeof(max_scale));                //0x14
			serde->WriteRecordData(&half_life, sizeof(half_life));                //0x18
			serde->WriteRecordData(&anim_speed, sizeof(anim_speed));              //0x1C
			serde->WriteRecordData(&effective_multi, sizeof(effective_multi));    //0x20
			serde->WriteRecordData(&bonus_hp, sizeof(bonus_hp));                  //0x24
			serde->WriteRecordData(&bonus_carry, sizeof(bonus_carry));            //0x28
			serde->WriteRecordData(&bonus_max_size, sizeof(bonus_max_size));      //0x2C
			serde->WriteRecordData(&smt_run_speed, sizeof(smt_run_speed));        //0x30

			serde->WriteRecordData(&NormalDamage, sizeof(NormalDamage));             //0x34
			serde->WriteRecordData(&SprintDamage, sizeof(SprintDamage));             //0x38
			serde->WriteRecordData(&FallDamage, sizeof(FallDamage));                 //0x3C
			serde->WriteRecordData(&HHDamage, sizeof(HHDamage));                     //0x40
			serde->WriteRecordData(&SizeVulnerability, sizeof(SizeVulnerability));   //0x44
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));               //0x48
			serde->WriteRecordData(&SizeReserve, sizeof(SizeReserve));               //0x4C

			serde->WriteRecordData(&target_scale_v, sizeof(target_scale_v));         //0x50
			serde->WriteRecordData(&scaleOverride, sizeof(scaleOverride));           //0x54

			serde->WriteRecordData(&stolen_attributes, sizeof(stolen_attributes));   //0x58
			serde->WriteRecordData(&stolen_health, sizeof(stolen_health));           //0x5C
			serde->WriteRecordData(&stolen_magick, sizeof(stolen_magick));           //0x60
			serde->WriteRecordData(&stolen_stamin, sizeof(stolen_stamin));           //0x64
		}



		if (!serde->OpenRecord(AllowPlayerVoreRecord, 0)) {
			log::error("Unable to open Allow Player Vore record to write cosave data.");
			return;
		}
		bool vore_allowplayervore = GetSingleton().vore_allowplayervore;
		serde->WriteRecordData(&vore_allowplayervore, sizeof(vore_allowplayervore));


		if (!serde->OpenRecord(AllowFollowerInteractions, 0)) {
			log::error("Unable to open Follower Interactions record to write cosave data.");
			return;
		}
		bool FollowerInteractions = GetSingleton().FollowerInteractions;
		serde->WriteRecordData(&FollowerInteractions, sizeof(FollowerInteractions));


		if (!serde->OpenRecord(VoreCombatOnlyRecord, 0)) {
			log::error("Unable to open Vore Combat Only record to write cosave data");
			return;
		}
		bool vore_combatonly = GetSingleton().vore_combatonly;
		serde->WriteRecordData(&vore_combatonly, sizeof(vore_combatonly));

		if (!serde->OpenRecord(AllowStaggerRecord, 1)) {
			log::error("Unable to open Allow Stagger record to write cosave data");
			return;
		}
		bool allow_stagger = GetSingleton().allow_stagger;
		serde->WriteRecordData(&allow_stagger, sizeof(allow_stagger));

		if (!serde->OpenRecord(EditVoiceFrequency, 1)) {
			log::error("Unable to open Override Actor Audio record to write cosave data");
			return;
		}
		bool edit_voice_frequency = GetSingleton().edit_voice_frequency;
		serde->WriteRecordData(&edit_voice_frequency, sizeof(edit_voice_frequency));

		if (!serde->OpenRecord(StompAiRecord, 1)) {
			log::error("Unable to open Stomp Ai record to write cosave data.");
			return;
		}
		bool Stomp_Ai = GetSingleton().Stomp_Ai;
		serde->WriteRecordData(&Stomp_Ai, sizeof(Stomp_Ai));


		if (!serde->OpenRecord(StolenAttributes, 1)) {
			log::error("Unable to open Stolen Attributes record to write cosave data");
			return;
		}
		float stolen_attributes = GetSingleton().stolen_attributes;
		serde->WriteRecordData(&stolen_attributes, sizeof(stolen_attributes));


		if (!serde->OpenRecord(Att_HealthStorage, 1)) {
			log::error("Unable to open Stolen Health Attributes record to write cosave data");
			return;
		}
		float stolen_health = GetSingleton().stolen_health;
		serde->WriteRecordData(&stolen_health, sizeof(stolen_health));

		if (!serde->OpenRecord(Att_StaminStorage, 1)) {
			log::error("Unable to open Stolen Stamina Attributes record to write cosave data");
			return;
		}
		float stolen_stamin = GetSingleton().stolen_stamin;
		serde->WriteRecordData(&stolen_stamin, sizeof(stolen_stamin));

		if (!serde->OpenRecord(Att_MagickStorage, 1)) {
			log::error("Unable to open Stolen Stamina Attributes record to write cosave data");
			return;
		}
		float stolen_magick = GetSingleton().stolen_magick;
		serde->WriteRecordData(&stolen_magick, sizeof(stolen_magick));

		/////////////////////////////////////////////////////////////////////////////////////////// Quest

		if (!serde->OpenRecord(Record_StolenSize, 1)) { // Stage 1
			log::error("Unable to open Stage 1 record to write cosave data");
			return;
		}
		float StolenSize = GetSingleton().StolenSize;
		serde->WriteRecordData(&StolenSize, sizeof(StolenSize));

		if (!serde->OpenRecord(Record_CrushCount, 1)) { // Stage 2
			log::error("Unable to open Stage 2 record to write cosave data");
			return;
		}
		float CrushCount = GetSingleton().CrushCount;
		serde->WriteRecordData(&CrushCount, sizeof(CrushCount));

		if (!serde->OpenRecord(Record_STNCount, 1)) { // Stage 3
			log::error("Unable to open Stage 3 record to write cosave data");
			return;
		}
		float STNCount = GetSingleton().STNCount;
		serde->WriteRecordData(&STNCount, sizeof(STNCount));

		if (!serde->OpenRecord(Record_HugStealCount, 1)) { // Stage 4
			log::error("Unable to open Stage 4 record to write cosave data");
			return;
		}
		float HugStealCount = GetSingleton().HugStealCount;
		serde->WriteRecordData(&HugStealCount, sizeof(HugStealCount));

		if (!serde->OpenRecord(Record_HandCrushed, 1)) { // Stage 5
			log::error("Unable to open Stage 5 record to write cosave data");
			return;
		}
		float HandCrushed = GetSingleton().HandCrushed;
		serde->WriteRecordData(&HandCrushed, sizeof(HandCrushed));

		if (!serde->OpenRecord(Record_VoreCount, 1)) { // Stage 6
			log::error("Unable to open Stage 6 record to write cosave data");
			return;
		}
		float VoreCount = GetSingleton().VoreCount;
		serde->WriteRecordData(&VoreCount, sizeof(VoreCount));

		if (!serde->OpenRecord(Record_GiantCount, 1)) { // stage 7
			log::error("Unable to open Stage 7 record to write cosave data");
			return;
		}
		float GiantCount = GetSingleton().GiantCount;
		serde->WriteRecordData(&GiantCount, sizeof(GiantCount));

		///////////////////////////////////////////////////////////////////////////////////////////
		if (!serde->OpenRecord(ActorsPanic, 1)) {
			log::error("Unable to open Actors Panic record to write cosave data");
			return;
		}
		bool actors_panic = GetSingleton().actors_panic;
		serde->WriteRecordData(&actors_panic, sizeof(actors_panic));

		if (!serde->OpenRecord(LegacySounds, 1)) {
			log::error("Unable to open Legacy Sounds record to write cosave data");
			return;
		}
		bool legacy_sounds = GetSingleton().legacy_sounds;
		serde->WriteRecordData(&legacy_sounds, sizeof(legacy_sounds));


		if (!serde->OpenRecord(Balance_SGP, 1)) {
			log::error("Unable to open Balance Mode: Size Gain penalty record to write cosave data");
			return;
		}

		float BalanceMode_SizeGain_Penalty = GetSingleton().BalanceMode_SizeGain_Penalty;
		serde->WriteRecordData(&BalanceMode_SizeGain_Penalty, sizeof(BalanceMode_SizeGain_Penalty));
		

		if (!serde->OpenRecord(Balance_SRB, 1)) {
			log::error("Unable to open Balance Mode: Shrink Rate Base record to write cosave data");
			return;
		}

		float BalanceMode_ShrinkRate_Base = GetSingleton().BalanceMode_ShrinkRate_Base;
		serde->WriteRecordData(&BalanceMode_ShrinkRate_Base, sizeof(BalanceMode_ShrinkRate_Base));

		if (!serde->OpenRecord(Balance_SRC, 1)) {
			log::error("Unable to open Balance Mode: Shrink Rate Combat record to write cosave data");
			return;
		}

		float BalanceMode_ShrinkRate_Combat = GetSingleton().BalanceMode_ShrinkRate_Combat;
		serde->WriteRecordData(&BalanceMode_ShrinkRate_Combat, sizeof(BalanceMode_ShrinkRate_Combat));

		if (!serde->OpenRecord(NPC_EffectImmunity, 1)) {
			log::error("Unable to open NPC Effect Immunity record to write cosave data");
			return;
		}
		bool NPCEffectImmunity = GetSingleton().NPCEffectImmunity;
		serde->WriteRecordData(&NPCEffectImmunity, sizeof(NPCEffectImmunity));

		if (!serde->OpenRecord(PC_EffectImmunity, 1)) {
			log::error("Unable to open PC Effect Immunity record to write cosave data");
			return;
		}
		bool PCEffectImmunity = GetSingleton().PCEffectImmunity;
		serde->WriteRecordData(&PCEffectImmunity, sizeof(PCEffectImmunity));

		if (!serde->OpenRecord(HostileToggle, 1)) {
			log::error("Unable to open Hostile Toggle Actors record to write cosave data");
			return;
		}
		bool hostile_toggle = GetSingleton().hostile_toggle;
		serde->WriteRecordData(&hostile_toggle, sizeof(hostile_toggle));

		if (!serde->OpenRecord(DeleteActors, 1)) {
			log::error("Unable to open Delete Actors record to write cosave data");
			return;
		}
		bool delete_actors = GetSingleton().delete_actors;
		serde->WriteRecordData(&delete_actors, sizeof(delete_actors));

		if (!serde->OpenRecord(HugsAiRecord, 1)) {
			log::error("Unable to open Hugs Ai record to write cosave data");
			return;
		}
		bool Hugs_Ai = GetSingleton().Hugs_Ai;
		serde->WriteRecordData(&Hugs_Ai, sizeof(Hugs_Ai));

		if (!serde->OpenRecord(ThighAiRecord, 1)) {
			log::error("Unable to open Thigh Ai record to write cosavedata");
			return;
		}
		bool Thigh_Ai = GetSingleton().Thigh_Ai;
		serde->WriteRecordData(&Thigh_Ai, sizeof(Thigh_Ai));

		if (!serde->OpenRecord(SandwichAiRecord, 1)) {
			log::error("Unable to open Sandwich Ai record to write cosave data.");
			return;
		}
		bool Sandwich_Ai = GetSingleton().Sandwich_Ai;
		serde->WriteRecordData(&Sandwich_Ai, sizeof(Sandwich_Ai));

		if (!serde->OpenRecord(ButtAiRecord, 1)) {
			log::error("Unable to open Butt Ai record to write cosave data.");
		}
		bool Butt_Ai = GetSingleton().Butt_Ai;
		serde->WriteRecordData(&Butt_Ai, sizeof(Butt_Ai));

		if (!serde->OpenRecord(KickAiRecord, 1)) {
			log::error("Unable to open Kick Ai record to write cosave data.");
			return;
		}
		bool Kick_Ai = GetSingleton().Kick_Ai;
		serde->WriteRecordData(&Kick_Ai, sizeof(Kick_Ai));

		if (!serde->OpenRecord(VoreAiRecord, 1)) {
			log::error("Unable to open Vore ai record to write cosave data.");
			return;
		}

		bool Vore_Ai = GetSingleton().Vore_Ai;
		serde->WriteRecordData(&Vore_Ai, sizeof(Vore_Ai));

		if (!serde->OpenRecord(ProgressionMult, 0)) {
			log::error("Unable to open Progression mult record to write cosave data");
			return;
		}
		float progression_multiplier = GetSingleton().progression_multiplier;
		serde->WriteRecordData(&progression_multiplier, sizeof(progression_multiplier));


		if (!serde->OpenRecord(XpMult, 0)) {
			log::error("Unable to open Experience Mult record to write cosave data");
			return;
		}

		float experience_mult = GetSingleton().experience_mult;
		serde->WriteRecordData(&experience_mult, sizeof(experience_mult));

		if (!serde->OpenRecord(SizeDamageMult, 0)) {
			log::error("Unable to open Damage mult record to write cosave data");
			return;
		}

		float size_related_damage_mult = GetSingleton().size_related_damage_mult;
		serde->WriteRecordData(&size_related_damage_mult, sizeof(size_related_damage_mult));

		if (!serde->OpenRecord(TremorScales, 0)) {
			log::error("Unable to open tremor scale record to write cosave data.");
			return;
		}

		float tremor_scale = GetSingleton().tremor_scale;
		serde->WriteRecordData(&tremor_scale, sizeof(tremor_scale));
		float npc_tremor_scale = GetSingleton().npc_tremor_scale;
		serde->WriteRecordData(&npc_tremor_scale, sizeof(npc_tremor_scale));

		if (!serde->OpenRecord(CamCollisions, 1)) {
			log::error("Unable to open camera collisions record to write cosave data.");
			return;
		}


		bool enable_trees = GetSingleton().camera_collisions.enable_trees;
		serde->WriteRecordData(&enable_trees, sizeof(enable_trees));
		bool enable_debris = GetSingleton().camera_collisions.enable_debris;
		serde->WriteRecordData(&enable_debris, sizeof(enable_debris));
		bool enable_terrain = GetSingleton().camera_collisions.enable_terrain;
		serde->WriteRecordData(&enable_terrain, sizeof(enable_terrain));
		bool enable_actor = GetSingleton().camera_collisions.enable_actor;
		serde->WriteRecordData(&enable_actor, sizeof(enable_actor));
		bool enable_static = GetSingleton().camera_collisions.enable_static;
		serde->WriteRecordData(&enable_static, sizeof(enable_static));
		float above_scale = GetSingleton().camera_collisions.above_scale;
		serde->WriteRecordData(&above_scale, sizeof(above_scale));


		Persi.TrackedCameraState.Save(serde);
		Persi.EnableCrawlPlayer.Save(serde);
		Persi.EnableCrawlFollower.Save(serde);

	}

	ActorData::ActorData() {
		// Uninit data
		// Make sure it is set elsewhere
	}

	ActorData::ActorData(Actor* actor) {
		// DEFAULT VALUES FOR NEW ACTORS
		auto scale = 1.0f;//get_scale(actor);
		this->native_scale = scale;
		this->visual_scale = scale;
		this->visual_scale_v = 0.0f;
		this->target_scale = scale;
		this->max_scale = DEFAULT_MAX_SCALE;
		this->half_life = DEFAULT_HALF_LIFE;
		this->anim_speed = 1.0f;
		this->bonus_hp = 0.0f;
		this->bonus_carry = 0.0f;
		this->bonus_max_size = 0.0f;
		this->smt_run_speed = 0.0f;
		this->NormalDamage = 1.0f;
		this->SprintDamage = 1.0f;
		this->FallDamage = 1.0f;
		this->HHDamage = 1.0f;
		this->SizeVulnerability = 0.0f;
		this->PAD_44 = 1.0f;
		this->SizeReserve = 0.0f;
		this->scaleOverride = -1.0f;

		this->stolen_attributes = 0.0f;
		this->stolen_health = 0.0f;
		this->stolen_magick = 0.0f;
		this->stolen_stamin = 0.0f;
	}

	ActorData* Persistent::GetActorData(Actor* actor) {
		if (!actor) {
			return nullptr;
		}
		return this->GetActorData(*actor);
	}

	ActorData* Persistent::GetActorData(Actor& actor) {
		std::unique_lock lock(this->_lock);
		auto key = actor.formID;
		ActorData* result = nullptr;
		try {
			result = &this->ActorDataMap.at(key);
		} catch (const std::out_of_range& oor) {
			// Add new
			if (!actor.Is3DLoaded()) {
				return nullptr;
			}
			auto scale = get_scale(&actor);
			if (scale < 0.0f) {
				return nullptr;
			}
			this->ActorDataMap.try_emplace(key, &actor);
			result = &this->ActorDataMap.at(key);
		}
		return result;
	}

	ActorData* Persistent::GetData(TESObjectREFR* refr) {
		if (!refr) {
			return nullptr;
		}
		return this->GetData(*refr);
	}

	ActorData* Persistent::GetData(TESObjectREFR& refr) {
		auto key = refr.formID;

		ActorData* result;

		try {
			result = &this->ActorDataMap.at(key);
		}
		catch (const std::out_of_range& oor) {
			return nullptr;
		}
		return result;
	}

	void Persistent::ResetActor(Actor* actor) {
		// Fired after a TESReset event
		//  This event should be when the game attempts to reset their
		//  actor values etc when the cell resets
		auto data = this->GetData(actor);
		if (data) {
			// 10.12.2023: changed visual and target scale to 1.0
			// Attempt to fix actors spawning with their old size basically
			data->visual_scale = 1.0f; //data->native_scale;
			data->target_scale = 1.0f; //data->native_scale;
			data->max_scale = DEFAULT_MAX_SCALE;
			data->visual_scale_v = 0.0f;
			data->half_life = DEFAULT_HALF_LIFE;
			data->anim_speed = 1.0f;
			data->bonus_hp = 0.0f;
			data->bonus_carry = 0.0f;
			data->bonus_max_size = 0.0f;
			data->smt_run_speed = 0.0f;
			data->NormalDamage = 1.0f;
			data->SprintDamage = 1.0f;
			data->FallDamage = 1.0f;
			data->HHDamage = 1.0f;
			data->SizeVulnerability = 0.0f;
			data->PAD_44 = 1.0f;
			data->SizeReserve = 0.0f;
			data->scaleOverride = -1.0f;

			data->stolen_attributes = 0.0f;
			data->stolen_health = 0.0f;
			data->stolen_magick = 0.0f;
			data->stolen_stamin = 0.0f;
		}
    ResetToInitScale(actor);
	}
}
