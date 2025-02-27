#pragma once

namespace GTS {

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
}