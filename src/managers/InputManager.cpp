#include "managers/animation/AnimationManager.hpp"
#include "managers/damage/CollisionDamage.hpp"
#include "managers/InputManager.hpp"
#include "data/runtime.hpp"
#include "data/plugin.hpp"

#include "data/time.hpp"

#include "Config/Keybinds.hpp"


using namespace SKSE;
using namespace RE;
using namespace GTS;

namespace GTS {

	//-----------------
	// ManagedInputEvent
	//-----------------

	ManagedInputEvent::ManagedInputEvent(const GTSInputEvent& a_event) {

		this->name = a_event.Event;
		float duration = a_event.Duration;
		this->exclusive = a_event.Exclusive;
		this->trigger = StringToEnum<TriggerType>(a_event.Trigger);
		this->blockinput = StringToEnum<BlockInputTypes>(a_event.BlockInput);
		this->minDuration = duration;
		this->startTime = 0.0;
		this->keys = {};

		const auto& keys = a_event.Keys;

		for (const auto& key: keys) {
			std::string upper_key = str_toupper(remove_whitespace(key));
			if (upper_key != "LEFT" && upper_key != "DIK_LEFT") {
				// This changes LEFTALT to LALT
				// But NOT LEFT into L
				replace_first(upper_key, "LEFT", "L");
			}
			if (upper_key != "RIGHT" && upper_key != "DIK_RIGHT") {
				replace_first(upper_key, "RIGHT", "R");
			}
			try {
				std::uint32_t key_code = NAMED_KEYS.at(upper_key);
				this->keys.emplace(key_code);
			} 
			catch (std::out_of_range e) {
				log::warn("Key named {}=>{} in {} was unrecongized.", key, upper_key, this->name);
				this->keys.clear();
				return; // Remove all keys and return so that this becomes an INVALID key entry and won't fire
			}
		}
	}

	float ManagedInputEvent::Duration() const {
		return static_cast<float>(Time::WorldTimeElapsed() - this->startTime);
	}

	float ManagedInputEvent::MinDuration() const {
		return this->minDuration;
	}

	void ManagedInputEvent::Reset() {
		this->startTime = Time::WorldTimeElapsed();
		this->state = InputEventState::Idle;
		this->primed = false;
	}

	bool ManagedInputEvent::IsOnUp() const {
		return this->trigger == TriggerType::Release;
	}

	bool ManagedInputEvent::SameGroup(const ManagedInputEvent& other) const {
		if (this->IsOnUp() && other.IsOnUp()) {
			return this->keys == other.keys;
		}
		return false;
	}

	bool ManagedInputEvent::AllKeysPressed(const std::unordered_set<std::uint32_t>& keys) {
		if (this->keys.empty()) {
			return false;
		}
		for (const auto& key : this->keys) {
			if (keys.find(key) == keys.end()) {
				// Key not found
				return false;
			}
		}
		return true;
	}

	bool ManagedInputEvent::OnlyKeysPressed(const std::unordered_set<std::uint32_t>& keys_in) {
		std::unordered_set<std::uint32_t> keys(keys_in); // Copy
		for (const auto& key : this->keys) {
			keys.erase(key);
		}
		return keys.size() == 0;
	}

	bool ManagedInputEvent::ShouldFire(const std::unordered_set<std::uint32_t>& a_gameInputKeys) {
		bool shouldFire = false;
		// Check based on keys and duration
		if (this->AllKeysPressed(a_gameInputKeys) && (!this->exclusive || this->OnlyKeysPressed(a_gameInputKeys))) {
			shouldFire = true;
		} 
		else {
			// Keys aren't held reset the start time of the button hold
			this->startTime = Time::WorldTimeElapsed();
			// and reset the state to idle
			this->state = InputEventState::Idle;
		}
		// Check based on duration
		if (shouldFire) {
			if (this->minDuration > 0.0) {
				// Turn it off if duration is not met
				shouldFire = this->Duration() > this->minDuration;
			}
		}

		// Check based on held and trigger state
		if (shouldFire) {
			this->primed = true;
			switch (this->state) {
				case InputEventState::Idle: {
					this->state = InputEventState::Held;
					switch (this->trigger) {
						// If once or continius start firing now
						case TriggerType::Once: {
							return true;
						}
						case TriggerType::Continuous: {
							return true;
						}
						case TriggerType::Release: {
							return false;
						}
						default: {
							log::error("Unexpected TriggerMode.");
							return false; // Catch if something goes weird
						}
					}
				}
				case InputEventState::Held: {
					switch (this->trigger) {
						// If once stop firing
						case TriggerType::Once: {
							return false;
						}
						case TriggerType::Continuous: {
							// if continous keep firing
							return true;
						}
						case TriggerType::Release: {
							// For release still do nothing
							return false;
						}
						default: {
							log::error("Unexpected TriggerMode.");
							return false; // Catch if something goes weird
						}
					}
				}
				default: {
					log::error("Unexpected InputEventState.");
					return false; // Catch if something goes weird
				}
			}
		} 
		else {
			if (this->primed) {
				this->primed = false;
				switch (this->trigger) {
					case TriggerType::Release: {
						// For release fire now that we have stopped pressing
						return true;
					}
					default: {
						return false;
					}
				}
			} 
			else {
				return false;
			}
		}
	}

	bool ManagedInputEvent::HasKeys() const {
		return !this->keys.empty();
	}

	std::string ManagedInputEvent::GetName() const {
		return this->name;
	}

	unordered_set<std::uint32_t> ManagedInputEvent::GetKeys() {
		return keys;
	}

	BlockInputTypes ManagedInputEvent::ShouldBlock() {
		return this->blockinput;
	}

	//-----------------
	// InputManager
	//-----------------

	std::vector<ManagedInputEvent> InputManager::LoadInputEvents() {

		KeybindsLocalCopy = Keybinds::GetSingleton().InputEvents;

		std::vector<ManagedInputEvent> results;

		for (const auto& GTSInputEvent : KeybindsLocalCopy) {

			ManagedInputEvent newData(GTSInputEvent);

			if (newData.HasKeys()) {
				results.push_back(newData);
			}

		}

		// Sort longest duration first
		ranges::sort(results,[](ManagedInputEvent const& a, ManagedInputEvent const& b) {
			return a.MinDuration() > b.MinDuration();
		});

		return results;
	}

	InputManager& InputManager::GetSingleton() noexcept {
		static InputManager instance;
		return instance;
	}

	void InputManager::RegisterInputEvent(std::string_view namesv, std::function<void(const ManagedInputEvent&)> callback, std::function<bool(void)> condition) {
		auto& me = InputManager::GetSingleton();
		std::string name(namesv);
		me.registedInputEvents.try_emplace(name, callback, condition);
		log::debug("Registered input event: {}", namesv);
	}

	void InputManager::Init() {

		Ready.store(false);

		try {
			InputManager::GetSingleton().keyTriggers = LoadInputEvents();
		} 
		catch (exception e) {
			log::error("Error Creating ManagedInputEvents: {}", e.what());
			return;
		} 

		log::info("Loaded {} key bindings", InputManager::GetSingleton().keyTriggers.size());
		
		Ready.store(true);
	}

	void InputManager::ProcessEvents(InputEvent** a_event) {

		std::unordered_set<uint32_t> KeysToBlock = {};
		std::unordered_set<std::uint32_t> gameInputKeys = {};
		RE::InputEvent* event = *a_event;
		RE::InputEvent* prev = nullptr;

		if (Plugin::AnyMenuOpen() || !a_event) {
			return;
		}

		if (!Ready.load()) {
			return;
		}

		//Get Current InputKeys
		for (auto event = *a_event; event; event = event->next) {
			//If the event is not a button, ignore.
			if (event->GetEventType() != INPUT_EVENT_TYPE::kButton) {
				continue;
			}

			//If the event is not a ButtonEvent or it is one but the event is "empty", ignore.
			ButtonEvent* buttonEvent = event->AsButtonEvent();
			if (!buttonEvent || (!buttonEvent->IsPressed() && !buttonEvent->IsUp())) {
				continue;
			}

			//If it is a ButtonEvent add it to to the list of pressed keys
			if (buttonEvent->device.get() == INPUT_DEVICE::kKeyboard) {
				auto key = buttonEvent->GetIDCode();
				gameInputKeys.emplace(key);
			}
			else if (buttonEvent->device.get() == INPUT_DEVICE::kMouse) {
				auto key = buttonEvent->GetIDCode();
				gameInputKeys.emplace(key + MOUSE_OFFSET);
			}
		}

		for (auto& trigger : this->keyTriggers) {
			// Store triggers in here that have been fired this frame
			std::vector<ManagedInputEvent*> firedTriggers; 
			auto blockInput = trigger.ShouldBlock();

			//Are all keys pressed for this trigger and are we allowed to selectively block?
			//if never: behavior defaults to old implementation
			if (trigger.AllKeysPressed(gameInputKeys)){
				//log::debug("AllkeysPressed for trigger {}", trigger.GetName());
				//Get the coresponding event data
				try {
					auto& eventData = this->registedInputEvents.at(trigger.GetName());

					if (blockInput == BlockInputTypes::Always) {
						//If force blocking is set block game input regardless of conditions
						std::unordered_set<uint32_t> KeysToAdd = std::unordered_set<uint32_t>(trigger.GetKeys());
						KeysToBlock.insert(KeysToAdd.begin(), KeysToAdd.end());

						if (eventData.condition != nullptr) {
							if (!eventData.condition()) {
								continue;
							}
						}

					}
					//The condition callback can be null, check before calling it.
					//In the case it's null input blocking or early continuing won't be done and the system will behave like previously unless its forced.
					else if (eventData.condition != nullptr) {
						//log::debug("condition exists {}", fmt::ptr(&eventData.condition));
						//Used to verify wether this trigger will actually end up doing anthing
						if (eventData.condition()) {
							//log::debug("condition is true for {}", trigger.GetName());
							//Need to make a copy here otherwise insert throws an assertion

							if (blockInput != BlockInputTypes::Never) {
								std::unordered_set<uint32_t> KeysToAdd = std::unordered_set<uint32_t>(trigger.GetKeys());
								//log::debug("ShouldBlock is true for {}", trigger.GetName());
								KeysToBlock.insert(KeysToAdd.begin(), KeysToAdd.end());
							}
						}
						else {
							//log::debug("Condition Was False For Event: {}", trigger.GetName());
							//If False Skip calling ShouldFire as there is no point in processing an event that won't do anything
							continue;
						}
					}
				}

				catch (std::out_of_range) {
					log::warn("Event {} was triggered but there is no event of that name", trigger.GetName());
					continue;
				}
			}

			//Handles Event tiggering conditions
			if (trigger.ShouldFire(gameInputKeys)) {
				bool groupAlreadyFired = false;
				for (auto firedTrigger : firedTriggers) {
					if (trigger.SameGroup(*firedTrigger)) {
						groupAlreadyFired = true;
						break;
					}
				}
				if (groupAlreadyFired) {
					trigger.Reset();
				}
				else {
					log::debug("Running event {}", trigger.GetName());
					firedTriggers.push_back(&trigger);
					try {
						auto& eventData = this->registedInputEvents.at(trigger.GetName());
						eventData.callback(trigger);
					}
					catch (std::out_of_range) {
						log::warn("Event {} was triggered but there is no event of that name", trigger.GetName());
					}
				}
			}
		}

		while (event != nullptr) {
			bool shouldDispatch = true;
			auto eventType = event->eventType;
			if (event->eventType == RE::INPUT_EVENT_TYPE::kButton) {
				const auto button = static_cast<RE::ButtonEvent*>(event);
				if (button) {
					uint32_t input = button->GetIDCode();
					if (KeysToBlock.find(input) != KeysToBlock.end()) {
						logger::debug("Blocked Input For Key {}", input);
							shouldDispatch = false;
					}
				}
			}

			RE::InputEvent* nextEvent = event->next;
			if (!shouldDispatch) {
				if (prev != nullptr) {
					prev->next = nextEvent;
				}
				else {
					*a_event = nextEvent;
				}
			}
			else {
				prev = event;
			}
			event = nextEvent;
		}
	}

	std::string InputManager::DebugName() {
		return "InputManager";
	}
}
