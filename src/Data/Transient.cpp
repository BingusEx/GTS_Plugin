#include "Data/Transient.hpp"

namespace GTS {

	Transient& Transient::GetSingleton() noexcept {
		static Transient Instance;
		return Instance;
	}

	TempActorData* Transient::GetData(TESObjectREFR* a_Object) {

		if (!a_Object) {
			return nullptr;
		}

		auto ActorKey = a_Object->formID;
		TempActorData* result = nullptr;

		try {
			result = &this->TempActorDataMap.at(ActorKey);
		}
		catch (const std::out_of_range&) {
			return nullptr;
		}

		return result;
	}

	TempActorData* Transient::GetActorData(Actor* actor) {

		std::unique_lock lock(this->TransientLock);

		if (!actor) {
			return nullptr;
		}

		auto ActorKey = actor->formID;

		try {
			return &this->TempActorDataMap.at(ActorKey);
		}
		catch (const std::out_of_range&) {

			// Try to add
			if (!actor) {
				return nullptr;
			}
			if (get_scale(actor) < 0.0f) {
				return nullptr;
			}
			TempActorData NewTempActorData = TempActorData(actor);
			auto [iter, inserted] = this->TempActorDataMap.try_emplace(ActorKey, NewTempActorData);
			return &(iter->second);

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

	//Disabled Update, It did nothing but create exceptions when a new actor was loaded.
	//Adding a new insertion on exception was not done previously so i guess its fine?

	//void Transient::Update() {

	//	for (Actor* actor : find_actors()) {

	//		if (!actor) {
	//			continue;
	//		}

	//		if (!actor->Is3DLoaded()) {
	//			continue;
	//		}

	//		FormID ActorKey = actor->formID;
	//		std::unique_lock lock(this->TransientLock);

	//		try {
	//			//Try getting Actor data.
	//			std::ignore = this->TempActorDataMap.at(ActorKey);
	//		}
	//		catch (const std::out_of_range&) {

	//			// Try to add
	//			if (!actor) {
	//				return;
	//			}
	//			if (get_scale(actor) < 0.0f) {
	//				return;
	//			}

	//			//Create New Data.
	//			TempActorData NewTempActorData = TempActorData(actor);
	//			this->TempActorDataMap.try_emplace(ActorKey, NewTempActorData);
	//		}
	//	}
	//}

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
