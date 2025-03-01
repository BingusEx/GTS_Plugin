
#include "Data/Persistent.hpp"
#include "Managers/GtsSizeManager.hpp"
#include "Utils/ItemDistributor.hpp"

namespace {


	inline const auto ActorDataRecord = _byteswap_ulong('ACTD');

	//TODO Move to new Data Type
	// Quest
	inline const auto Record_StolenSize = _byteswap_ulong('QSSR');
	inline const auto Record_CrushCount = _byteswap_ulong('QCCR');
	inline const auto Record_STNCount = _byteswap_ulong('QSTR');
	inline const auto Record_HugStealCount = _byteswap_ulong('QHSR');
	inline const auto Record_HandCrushed = _byteswap_ulong('QHCR');
	inline const auto Record_VoreCount = _byteswap_ulong('QVRR');
	inline const auto Record_GiantCount = _byteswap_ulong('QGCR');
	//

	inline constexpr float DEFAULT_MAX_SCALE = 65535.0f;
	inline constexpr float DEFAULT_HALF_LIFE = 1.0f;
	inline constexpr float DUMMY_FLOAT = 0.0f;
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

		DistributeChestItems();
		FixAnimationsAndCamera(); // Call it from ActorUtils, needed to fix Grab anim on save-reload

		auto& Persi = Persistent::GetSingleton();

		while (serde->GetNextRecordInfo(type, version, size)) {

			Persi.TrackedCameraState.Load(serde, type, version, size);
			Persi.EnableCrawlPlayer.Load(serde, type, version, size);
			Persi.EnableCrawlFollower.Load(serde, type, version, size);
			Persi.GTSMassBasedSize.Load(serde, type, version, size);
			Persi.GTSExtraPotionSize.Load(serde, type, version, size);
			Persi.GTSGlobalSizeLimit.Load(serde, type, version, size);

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

						ActorData Data = {};

						//Data.PAD_00;
						Data.visual_scale = visual_scale;
						Data.visual_scale_v = visual_scale_v;
						Data.target_scale = target_scale;
						Data.max_scale = max_scale;
						Data.half_life = half_life;
						Data.anim_speed = anim_speed;
						//Data.PAD_1C;
						//Data.PAD_20;
						//Data.PAD_24;
						//Data.PAD_28;
						Data.smt_run_speed = smt_run_speed;
						Data.NormalDamage = NormalDamage;
						Data.SprintDamage = SprintDamage;
						Data.FallDamage = FallDamage;
						Data.HHDamage = HHDamage;
						//Data.PAD_40;
						Data.SizeReserve = SizeReserve;
						Data.target_scale_v = target_scale_v;
						//Data.PAD_50;
						Data.stolen_attributes = stolen_attributes;
						Data.stolen_health = stolen_health;
						Data.stolen_magick = stolen_magick;
						Data.stolen_stamin = stolen_stamin;

						TESForm* actor_form = TESForm::LookupByID<Actor>(NewActorFormID);

						if (actor_form) {

							Actor* actor = skyrim_cast<Actor*>(actor_form);

							if (actor) {
								Persistent::GetSingleton().ActorDataMap.insert_or_assign(NewActorFormID, Data);
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

		for (auto const& [form_id_t, data] : GetSingleton().ActorDataMap) {

			FormID form_id = form_id_t;
			//data.PAD_00;
			const float visual_scale = data.visual_scale;
			const float visual_scale_v = data.visual_scale_v;
			const float target_scale = data.target_scale;
			const float max_scale = data.max_scale;
			const float half_life = data.half_life;
			const float anim_speed = data.anim_speed;
			//data.PAD_1C;
			//data.PAD_20;
			//data.PAD_24;
			//data.PAD_28;
			const float smt_run_speed = data.smt_run_speed;
			const float NormalDamage = data.NormalDamage;
			const float SprintDamage = data.SprintDamage;
			const float FallDamage = data.FallDamage;
			const float HHDamage = data.HHDamage;
			//data.PAD_40;
			//data.PAD_44;
			const float SizeReserve = data.SizeReserve;
			const float target_scale_v = data.target_scale_v;
			//data.PAD_50;
			const float stolen_attributes = data.stolen_attributes;
			const float stolen_health = data.stolen_health;
			const float stolen_magick = data.stolen_magick;
			const float stolen_stamin = data.stolen_stamin;

			serde->WriteRecordData(&form_id, sizeof(form_id));                    //0x00
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));            //0x04
			serde->WriteRecordData(&visual_scale, sizeof(visual_scale));          //0x08
			serde->WriteRecordData(&visual_scale_v, sizeof(visual_scale_v));      //0x0C
			serde->WriteRecordData(&target_scale, sizeof(target_scale));          //0x10
			serde->WriteRecordData(&max_scale, sizeof(max_scale));                //0x14
			serde->WriteRecordData(&half_life, sizeof(half_life));                //0x18
			serde->WriteRecordData(&anim_speed, sizeof(anim_speed));              //0x1C
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));            //0x20
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));            //0x24
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));            //0x28
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));               //0x2C
			serde->WriteRecordData(&smt_run_speed, sizeof(smt_run_speed));           //0x30
			serde->WriteRecordData(&NormalDamage, sizeof(NormalDamage));             //0x34
			serde->WriteRecordData(&SprintDamage, sizeof(SprintDamage));             //0x38
			serde->WriteRecordData(&FallDamage, sizeof(FallDamage));                 //0x3C
			serde->WriteRecordData(&HHDamage, sizeof(HHDamage));                     //0x40
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));               //0x44
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));               //0x48
			serde->WriteRecordData(&SizeReserve, sizeof(SizeReserve));               //0x4C
			serde->WriteRecordData(&target_scale_v, sizeof(target_scale_v));         //0x50
			serde->WriteRecordData(&DUMMY_FLOAT, sizeof(DUMMY_FLOAT));               //0x54
			serde->WriteRecordData(&stolen_attributes, sizeof(stolen_attributes));   //0x58
			serde->WriteRecordData(&stolen_health, sizeof(stolen_health));           //0x5C
			serde->WriteRecordData(&stolen_magick, sizeof(stolen_magick));           //0x60
			serde->WriteRecordData(&stolen_stamin, sizeof(stolen_stamin));           //0x64
		}


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

		Persi.TrackedCameraState.Save(serde);
		Persi.EnableCrawlPlayer.Save(serde);
		Persi.EnableCrawlFollower.Save(serde);
		Persi.GTSMassBasedSize.Save(serde);
		Persi.GTSExtraPotionSize.Save(serde);
		Persi.GTSGlobalSizeLimit.Save(serde);
	}

	ActorData::ActorData() = default;

	ActorData::ActorData(Actor* actor) {
		// Default values for new actors
		//this->PAD_00;
		this->visual_scale = 1.0f;
		this->visual_scale_v = 0.0f;
		this->target_scale = 1.0f;
		this->max_scale = DEFAULT_MAX_SCALE;
		this->half_life = DEFAULT_HALF_LIFE;
		this->anim_speed = 1.0f;
		//this->PAD_1C;
		//this->PAD_20;
		//this->PAD_24;
		//this->PAD_28;
		this->smt_run_speed = 0.0f;
		this->NormalDamage = 1.0f;
		this->SprintDamage = 1.0f;
		this->FallDamage = 1.0f;
		this->HHDamage = 1.0f;
		//this->PAD_40;
		//this->PAD_44;
		this->SizeReserve = 0.0f;
		//this->PAD_50;
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
		}
		catch (const std::out_of_range&) {
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
		catch (const std::out_of_range&) {
			return nullptr;
		}
		return result;
	}

	void Persistent::ResetActor(Actor* actor) {
		// Fired after a TESReset event
		// This event should be when the game attempts to reset their
		// actor values etc when the cell resets
		auto data = this->GetData(actor);
		if (data) {
			//data->PAD_00;
			data->visual_scale = 1.0f; 
			data->target_scale = 1.0f; 
			data->max_scale = DEFAULT_MAX_SCALE;
			data->visual_scale_v = 0.0f;
			data->half_life = DEFAULT_HALF_LIFE;
			data->anim_speed = 1.0f;
			//data->PAD_1C;
			//data->PAD_20;
			//data->PAD_24;
			//data->PAD_28;
			data->smt_run_speed = 0.0f;
			data->NormalDamage = 1.0f;
			data->SprintDamage = 1.0f;
			data->FallDamage = 1.0f;
			data->HHDamage = 1.0f;
			//data->PAD_40;
			//data->PAD_44;
			data->SizeReserve = 0.0f;
			//data->PAD_50;
			data->stolen_attributes = 0.0f;
			data->stolen_health = 0.0f;
			data->stolen_magick = 0.0f;
			data->stolen_stamin = 0.0f;
		}
		ResetToInitScale(actor);
	}
}
