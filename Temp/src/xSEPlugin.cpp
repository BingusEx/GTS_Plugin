#include "managers/animation/Utils/CooldownManager.hpp"

#include "hooks/hooks.hpp"
#include "papyrus/papyrus.hpp"
#include "data/plugin.hpp"

#include "managers/register.hpp"
#include "Managers/Input/InputManager.hpp"
#include "UI/DebugAPI.hpp"
#include "data/runtime.hpp"
#include "data/persistent.hpp"
#include "data/transient.hpp"

#include "Config/Config.hpp"
#include "API/APIManager.hpp"

using namespace RE::BSScript;

namespace {

	// This function allocates a new console, redirects the standard streams,
	// and enables ANSI escape sequences for colored output.
	void AllocateConsole() {

		// Allocate a new console for the calling process.
		if (!AllocConsole()) {
			std::cerr << "Failed to allocate console!" << '\n';
			return;
		}

		// Redirect unbuffered STDOUT to the console.
		FILE* fpOut = nullptr;
		if (freopen_s(&fpOut, "CONOUT$", "w", stdout) != 0) {
			std::cerr << "Failed to redirect stdout!" << '\n';
		}

		// Redirect unbuffered STDIN to the console.
		FILE* fpIn = nullptr;
		if (freopen_s(&fpIn, "CONIN$", "r", stdin) != 0) {
			std::cerr << "Failed to redirect stdin!" << '\n';
		}

		// Redirect unbuffered STDERR to the console.
		FILE* fpErr = nullptr;
		if (freopen_s(&fpErr, "CONOUT$", "w", stderr) != 0) {
			std::cerr << "Failed to redirect stderr!" << '\n';
		}


		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		// Enable ANSI escape sequences for colored output (requires Windows 10+).
		DWORD mode = 0;
		if (GetConsoleMode(hConsole, &mode)) {
			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			SetConsoleMode(hConsole, mode);
		}

		// Synchronize the C++ standard streams with the C standard streams.
		std::ios::sync_with_stdio();
		
	}

	void VersionCheck(const LoadInterface* a_skse) {
		if (a_skse->RuntimeVersion() < SKSE::RUNTIME_SSE_1_5_80 || REL::Module::IsVR()) {
			ReportAndExit("This mod does not support Skyrim VR or versions of Skyrim older than 1.5.80.");
		}
	}

	void PrintStartupBanner() {
		Cprint("[GTSPlugin.dll]: [ Giantess Mod v2.0.1.1 was succesfully initialized. Waiting for New Game/Save Load. ]");
		Cprint("[GTSPlugin.dll]: Dll Build Date: {} {}", __DATE__, __TIME__);
		Cprint("[GTSPlugin.dll]: Git Info:");
		Cprint("\t -- Commit: {}", git_CommitSubject());
		Cprint("\t -- SHA1: {}", git_CommitSHA1());
		Cprint("\t -- Date: {}", git_CommitDate());
		Cprint("\t -- LocalChanges: {}", git_AnyUncommittedChanges() ? "Yes" : "No");
	}

	void InitializeLogging() {
		auto path = GTS::log_directory();

		if (!path) {
			ReportAndExit("Unable to lookup SKSE logs directory.");
		}
		*path /= PluginDeclaration::GetSingleton()->GetName();
		*path += L".log";

		std::shared_ptr <spdlog::logger> log;

		if (IsDebuggerPresent()) {
			log = std::make_shared <spdlog::logger>(
				"Global", std::make_shared <spdlog::sinks::msvc_sink_mt>());

			log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
		}
		else {

			#ifdef GTSDEBUG
				auto file_sink = std::make_shared <spdlog::sinks::basic_file_sink_mt>(path->string(), true);
				file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

				auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>(spdlog::color_mode::always);
				console_sink->set_pattern("[GTS] [%l] [%s:%#] %v");

				log = std::make_shared <spdlog::logger>(spdlog::logger("Global", { console_sink, file_sink }));
			#else
				log = std::make_shared <spdlog::logger>("Global", std::make_shared <spdlog::sinks::basic_file_sink_mt>(path->string(), true));
				log->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");
			#endif

		}

		spdlog::set_default_logger(std::move(log));
		spdlog::set_level(spdlog::level::level_enum::trace);
		spdlog::flush_on(spdlog::level::level_enum::trace);

	}

	void InitializeMessaging() {

		if (!GetMessagingInterface()->RegisterListener([](MessagingInterface::Message *message) {
			switch (message->type) {

				// Called after all plugins have finished running SKSEPlugin_Load.
				case MessagingInterface::kPostLoad:     
				{ 
					//RegisterAPIs();
					break;
				}

				// Called after all kPostLoad message handlers have run.
				case MessagingInterface::kPostPostLoad: 
				{ 
					break;
				}

				// Called when all game data has been found.
				case MessagingInterface::kInputLoaded:   
				{
					break;
				}

				// All ESM/ESL/ESP plugins have loaded, main menu is now active.
				case MessagingInterface::kDataLoaded:  
				{    
					EventDispatcher::DoDataReady();
					InputManager::GetSingleton().Init();
					APIManager::GetSingleton().Register();

					PrintStartupBanner();
					break;
				}

				// Skyrim game events.
				// Player's selected save game has finished loading.
				case MessagingInterface::kPostLoadGame:  
				{
					Plugin::SetInGame(true);
					Cprint(" [GTSPlugin.dll]: [ Giantess Mod was succesfully initialized and loaded. ]");
					break;
				}

				// Player starts a new game from main menu.
				case MessagingInterface::kNewGame:
				{
					Plugin::SetInGame(true);
					EventDispatcher::DoReset();
					Cprint(" [GTSPlugin.dll]: [ Giantess Mod was succesfully initialized and loaded. ]");
					break;
				}

				// Player selected a game to load, but it hasn't loaded yet.
				// Data will be the name of the loaded save.
				case MessagingInterface::kPreLoadGame: 
				{
					Plugin::SetInGame(false);
					EventDispatcher::DoReset();
					break;
				}

				// The player has saved a game.
				case MessagingInterface::kSaveGame:
				{
					break;
				}

				// The player deleted a saved game from within the load menu.
				// Data will be the save name.
				case MessagingInterface::kDeleteGame:
				{
					break;
				} 

			}
		})) {
			ReportAndExit("Unable to register message listener.");
		}
	}
}

static void InitializeSerialization() {
	log::trace("Initializing cosave serialization...");
	auto* serde = GetSerializationInterface();
	serde->SetUniqueID(_byteswap_ulong('GTSP'));
	serde->SetSaveCallback(Persistent::OnGameSaved);
	serde->SetRevertCallback(Persistent::OnRevert);
	serde->SetLoadCallback(Persistent::OnGameLoaded);
	log::info("Cosave serialization initialized.");
}

static void InitializePapyrus() {
	log::trace("Initializing Papyrus binding...");
	if (GetPapyrusInterface()->Register(GTS::register_papyrus)) {
		log::info("Papyrus functions bound.");
	} else {
		ReportAndExit("Failure to register Papyrus bindings.");
	}
}

static void InitializeEventSystem() {
	EventDispatcher::AddListener(&DebugOverlayMenu::GetSingleton());
	EventDispatcher::AddListener(&Runtime::GetSingleton()); // Stores spells, globals and other important data
	EventDispatcher::AddListener(&Persistent::GetSingleton());
	EventDispatcher::AddListener(&Transient::GetSingleton());
	EventDispatcher::AddListener(&CooldownManager::GetSingleton());
	EventDispatcher::AddListener(&TaskManager::GetSingleton());
	EventDispatcher::AddListener(&SpringManager::GetSingleton());

	log::info("Adding Default Listeners");

	RegisterManagers();
}

static void PrintPluginInfo() {

	logger::info("SKSEPluginLoad... ");
	logger::info("Dll Build Date: {} {}", __DATE__, __TIME__);

	std::string git_commit = fmt::format("\t -- Commit: {}", git_CommitSubject());
	std::string git_sha1 = fmt::format("\t -- SHA1: {}", git_CommitSHA1());
	std::string git_date = fmt::format("\t -- Date: {}", git_CommitDate());
	std::string git_ditry = fmt::format("\t -- LocalChanges: {}", git_AnyUncommittedChanges() ? "Yes" : "No");

	logger::info("Git Info:\n{}\n{}\n{}\n{}", git_commit, git_sha1, git_date, git_ditry);

}

static void SetLogLevel() {

	try {
		log::info("Getting Logger Config...");
		const auto& debugConfig = Config::GetAdvanced();
		log::info("Config Loaded from settings struct: Print: {} Flush: {}",debugConfig.sLogLevel, debugConfig.sFlushLevel);

		spdlog::set_level(spdlog::level::from_str(debugConfig.sLogLevel));
		spdlog::flush_on(spdlog::level::from_str(debugConfig.sFlushLevel));
	}
	catch (exception& e){
		logger::critical("Could not load spdlog settings from config struct", e.what());
		ReportAndExit("Could not load spdlog settings from config struct");
	}
}

SKSEPluginLoad(const LoadInterface * a_skse){

	//This hack is needed because debug builds of commonlib combletly shit the bed during trampoline hooks
	//Destination pointers for write call and write branch suddenly forget to add offsets to skyrims base image address.
	//Why??? who tf knows why...
	//So we instead build with the relwithdebinfo preset when using the debug and debug-eha presets, but pass all debug flags to the compiler when doing so...
	//This results in this dll being built with full debug options but commonlib and other libraries being built as release...
	//I mean is this good? No. But does it finnaly allow us to propperly debug this dll? Yes.

	//If you see a 32+ mb dll being built there's a 100% chance it will ctd at the first hook.
	//"Normal" debug dlls should be around 21-23mb as of 15-02-2025

	#ifdef GTSDEBUG
		AllocateConsole();
		ReportInfo("GTSplugin debug build, Attach the debugger and press OK.");
		std::cout << "GTSPlugin Debug Build" << '\n';
	#endif

	InitializeLogging();
	PrintPluginInfo();
	SetLogLevel();

	Init(a_skse);
	VersionCheck(a_skse);

	InitializeMessaging();
	Hooks::Install();
	InitializePapyrus();
	InitializeSerialization();
	InitializeEventSystem();

	logger::info("SKSEPluginLoad OK");

	return true;
}