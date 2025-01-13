#include "Config.hpp"

using namespace Gts;

const Config& Config::GetSingleton() noexcept {
	static Config instance;

	static std::atomic_bool initialized;
	static std::latch latch(1);
	if (!initialized.exchange(true)) {
		log::info("Parsing toml");
		const auto data = toml::parse(R"(Data\SKSE\Plugins\GtsPlugin.toml)");
		log::info("Toml Parsed");
		log::info("Loading Config");
		instance = Config(data);
		log::info("Loaded Config");
		latch.count_down();
	}
	latch.wait();

	return instance;
}

namespace Gts {
	Debug::Debug(const toml::value& data) {
		std::string logLevel = toml::find_or<std::string>(data, "logLevel", "info");
		std::string flushLevel = toml::find_or<std::string>(data, "flushLevel", "trace");
		this->_logLevel = spdlog::level::from_str(logLevel);
		this->_flushLevel = spdlog::level::from_str(flushLevel);
		this->_shouldProfile = toml::find_or<bool>(data, "profile", false);
	}

	Frame::Frame(const toml::value& data) {
		int step = toml::find_or<int>(data, "step", 0);
		int initDelay = toml::find_or<int>(data, "initDelay", 0);
		this->_step = step;
		this->_initDelay = initDelay;
	}

	Voice::Voice(const toml::value& data) {
		this->_maxVoiceFrequency = toml::find_or<float>(data, "maxVoiceFrequency", 1.0f);
	}

	UtilBools::UtilBools(const toml::value& data) {
		this->_ignoreFemaleCheck = toml::find_or<bool>(data, "ignoreFemaleCheck", false);
	}

	Tremor::Tremor(const toml::value& data) {
		this->_method = toml::find_or<std::string>(data, "method", "linear");
		this->_halfScale = toml::find_or<float>(data, "halfScale", 0.91f);
		this->_powerAtMin = toml::find_or<float>(data, "powerAtMin", 0.0f);
		this->_powerAtMax = toml::find_or<float>(data, "powerAtMax", 8.25f);
		this->_maxScale = toml::find_or<float>(data, "maxScale", 30.0f);
		this->_minScale = toml::find_or<float>(data, "minScale", 0.0f);
	}

	Config::Config(const toml::value& data) {
		this->_debug =  toml::find<Debug>(data, "debug");
		this->_frame =  toml::find<Frame>(data, "frame");
		this->_tremor =  toml::find<Tremor>(data, "tremor");
		this->_voice =  toml::find<Voice>(data, "voice");
		this->_utilBools = toml::find<UtilBools>(data, "UtilBools");
	}
}
