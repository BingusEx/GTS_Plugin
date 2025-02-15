#pragma once

#include <toml.hpp>
#include <magic_enum/magic_enum.hpp>

#include "Config/ConfigUtil.hpp"
#include "Config/SettingsList.hpp"
#include "Utils/MessageboxUtil.hpp"


//-------------------------------------------------------------------------------------------------------------------
//  ENUMS ----- Assumed to be the reference values 
//  magic_enum will use to convert an enum to a string representation for serialization (Saving The TOML)
//-------------------------------------------------------------------------------------------------------------------

//TODO THESE SHOULD NOT BE HERE
enum class CameraTrackingUsr {
    kNone,
    kSpine,
    kClavicle,
    kBreasts_01,
    kBreasts_02,
    kBreasts_03, // 3BBB tip
    kNeck,
    kButt,
};

enum class CameraModeTP {
    kDisabled,
    kNormal,
    kAlternative,
    kFootLeft,
    kFootRight,
    kFeetCenter
};

enum class CameraModeFP {
    kNormal,
    kCombat,
    kLoot
};

enum class DisplayUnit {
    kMetric,
    kImperial,
    kMammoth
};

enum class SizeMode {
    kNormal,
    kMassBased
};

enum class GameMode {
    kNone,
    kGrow,
    kShrink,
    kCombatGrowth,
    kSlowCombatGrowth,          //Curse of growth already exists as random growth, its superfluous
    kCurseOfTheGiantess,        //<------- TODO Implement these... AKA: Just take them from my dll
    kCurseOfDiminishing,
    kSizeLocked //<-------------- Combines both
};

namespace GTS {

    class Config {

        private:

        //Create structs with default values.
        //These act as sane defaults in case new data is loaded or the toml itself is corrupted.
        SettingsGeneral General = {};
        SettingsAdvanced Advanced = {};
        SettingsAI AI = {};
        SettingsAudio Audio = {};
        SettingsBalance Balance = {};
        SettingsCamera Camera = {};
        SettingsGameplay Gameplay{};
        SettingsUI GtsUI = {};
        SettingsHidden Hidden = {};

        template <typename T>
        constexpr std::string_view GetStructName(const T&) {
            return reflect::type_name<T>();
        }

	    #define REGISTER_STRUCT_NAME(TYPE, NAME)                        \
	    template <>                                                     \
	    constexpr std::string_view GetStructName<TYPE>(const TYPE&) {   \
			return NAME;                                                \
		}

        //Too much effort to make a parser for it, So Define the names here.
        REGISTER_STRUCT_NAME(SettingsGeneral, "General");
        REGISTER_STRUCT_NAME(SettingsAdvanced, "Advanced");
        REGISTER_STRUCT_NAME(SettingsAI, "AI");
        REGISTER_STRUCT_NAME(SettingsAudio, "Audio");
        REGISTER_STRUCT_NAME(SettingsBalance, "Balance");
        REGISTER_STRUCT_NAME(SettingsCamera, "Camera");
        REGISTER_STRUCT_NAME(SettingsGameplay, "Gameplay");
        REGISTER_STRUCT_NAME(SettingsUI, "GtsUI");
        REGISTER_STRUCT_NAME(SettingsHidden, "Hidden");

        const std::string _Subfolder = "Data\\SKSE\\Plugins\\GTSPlugin";
        const std::string _ConfigFile = "Settings.toml";

        //Currentpath Resolves to the Skyrim root folder where the .exe is.
        const std::filesystem::path ConfigFile = std::filesystem::current_path() / _Subfolder / _ConfigFile;

        toml::basic_value<toml::ordered_type_config> TomlData;
        std::mutex _ReadWriteLock;

        Config() = default;
        Config(const Config&) = delete;
        Config& operator=(const Config&) = delete;

        template<typename T>
        [[nodiscard]] bool LoadStructFromTOML(const auto& a_toml, T& a_data);

        template<typename T>
        [[nodiscard]] bool UpdateTOMLFromStruct(auto& a_toml, T& a_data);

        [[nodiscard]] bool SaveTOMLToFile(const auto& a_toml, const std::filesystem::path& a_file);

        public:

        //Static Accessors (Helpers)

        [[nodiscard]] static inline SettingsGeneral& GetGeneral() {
            return GetSingleton().General;
        }

        [[nodiscard]] static inline SettingsAdvanced& GetAdvanced() {

            return GetSingleton().Advanced;
        }

        [[nodiscard]] static inline SettingsAI& GetAI() {
            return GetSingleton().AI;
        }

        [[nodiscard]] static inline SettingsAudio& GetAudio() {
            return GetSingleton().Audio;
        }

        [[nodiscard]] static inline SettingsBalance& GetBalance() {
            return GetSingleton().Balance;
        }

        [[nodiscard]] static inline SettingsCamera& GetCamera() {
            return GetSingleton().Camera;
        }

        [[nodiscard]] static inline SettingsGameplay& GetGameplay() {
            return GetSingleton().Gameplay;
        }

        [[nodiscard]] static inline SettingsUI& GetUI() {
            return GetSingleton().GtsUI;
        }

        [[nodiscard]] static inline SettingsHidden& GetHidden() {
            return GetSingleton().Hidden;
        }

        [[nodiscard]] static inline Config& GetSingleton() {
            static Config Instance;

            static std::atomic_bool Initialized;
            static std::latch Latch(1);

            if (!Initialized.exchange(true)) {
                logger::info("Loading TOML Settings in .ctor...");

                if (!Instance.LoadSettings()) {
                    ReportInfo("Settings.toml is either invalid or corrupted.\n"
							   "Click OK to clear out the settings file.\n"
							   "This will reset the mod's settings."
                    );

                    Instance.ResetToDefaults();

                    //Delete the config file
                    if (!DeleteFile(Instance.ConfigFile)) {
                        ReportAndExit("Could not delete Settings.toml\n"
									  "Check GTSPlugin.log for more info.\n"
									  "The game will now close."
                        );
                    }

                    //Recreate the config file and start with a fresh table.
                    if (!CheckFile(Instance.ConfigFile)) {
                        ReportAndExit("Could not create a blank Settings.toml file.\n"
                                      "Check GTSPlugin.log for more info.\n"
                                      "The game will now close."
                        );
                    }
                }

                //Incase the File is empty/missing newly added data...
                //Explicitly Ignore the [[Nodiscard]]
                std::ignore = Instance.SaveSettings();

                logger::info(".ctor Load OK");

                Latch.count_down();
            }
            Latch.wait();

            return Instance;
        }

        [[nodiscard]] bool LoadSettings();

        [[nodiscard]] bool SaveSettings();

        void ResetToDefaults();

    };
}