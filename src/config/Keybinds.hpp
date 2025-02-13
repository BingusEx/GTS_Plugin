#pragma once
#include "config/ConfigUtil.hpp"
#include "toml.hpp"
#include "KeybindList.hpp"

//---------------
// INPUT EVENT
//---------------

namespace Input {

    class Keybinds {
        public:

        std::vector<GTSInputEvent> InputEvents = {};

        [[nodiscard]] static inline Keybinds& GetSingleton(){
            static Keybinds Instance;

            static std::atomic_bool Initialized;
            static std::latch Latch(1);
            
            if (!Initialized.exchange(true)) {
                logger::info("Loading TOML Keybinds in .ctor...");
    
                if(!Instance.LoadKeybinds()){
                    MessageBoxA(nullptr,"Input.toml is either invalid or corrupted. Click OK to clear out the file.\nThis will reset your keybinds.","Size Matters - GTSPlugin.dll",MB_OK);
            
                    Instance.ResetKeybinds();
                    
                    //Delete the config file
                    if(!DeleteFile(Instance.InputFile)){
                        MessageBoxA(nullptr,"Could not delete Input.toml\nCheck GTSPlugin.log for more info.\nThe game will now close.", "Size Matters - GTSPlugin.dll", MB_OK);
                        TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
                    }
                    
                    //Recreate the config file and start with a fresh table.
                    if(!CheckFile(Instance.InputFile)){
                        MessageBoxA(nullptr,"Could not create a blank Input.toml file.\nCheck GTSPlugin.log for more info.\nThe game will now close.", "Size Matters - GTSPlugin.dll", MB_OK);
                        TerminateProcess(GetCurrentProcess(), EXIT_FAILURE);
                    }
                }
    
                //Incase the File is empty/missing newly added data...
                //Explicitly Ignore the [[Nodiscard]]
                std::ignore = Instance.SaveKeybinds();
    
                logger::info(".ctor Load OK");
    
                Latch.count_down();
            }
            Latch.wait();

            return Instance;
        }

        [[nodiscard]] bool LoadKeybinds();
        [[nodiscard]] bool SaveKeybinds();
        void ResetKeybinds();

        private:
        Keybinds() = default;
        Keybinds(const Keybinds&) = delete;
        Keybinds& operator=(const Keybinds&) = delete;

        const std::string _Subfolder = "Data\\SKSE\\Plugins\\GTSPlugin"; 
        const std::string _ConfigFile = "Input.toml";
        //CURRENTPATH IS THE SKYRIM ROOT FOLDER
        const std::filesystem::path InputFile = std::filesystem::current_path() / _Subfolder / _ConfigFile;
    
        toml::basic_value<toml::ordered_type_config> TomlData;
        std::mutex _ReadWriteLock;
    };
}