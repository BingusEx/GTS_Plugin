#pragma once

#include "Config/Keybinds.hpp"

namespace GTS {

	enum class InputEventState {
		Idle,
		Held,
	};

	class ManagedInputEvent {
		public:
			// Construct from toml::table (toml11)
			ManagedInputEvent(const GTSInputEvent& a_event);

			// Return time since it was first pressed
			float Duration() const;

			// Will take a key list and process if the event should fire.
			//   will return true if the events conditions are met
			bool ShouldFire(const std::unordered_set<std::uint32_t>& keys);

			// Returns true if all keys are pressed this frame
			//  Not taking into account things like duration
			bool AllKeysPressed(const std::unordered_set<std::uint32_t>& keys);

			// Returns true if ONLY the specicified keys are pressed this frame
			//   Not taking into account things like duration
			bool OnlyKeysPressed(const std::unordered_set<std::uint32_t>& keys);

			// Resets the timer and all appropiate state variables
			void Reset();

			// Returns the duration required for the event to fire
			float MinDuration() const;

			// Returns if the event is a onup event
			bool IsOnUp() const;

			std::string GetName() const;

			// Check if this is an On key up event
			//bool IsOnUp();
			bool HasKeys() const;

			// Checks if this key is the same as another in terms
			// of mutaally exclusive triggers
			bool SameGroup(const ManagedInputEvent& other) const;

			unordered_set<std::uint32_t> GetKeys();

			BlockInputTypes ShouldBlock();
		private:

			double startTime = 0.0;
			bool primed = false; // Used for release events. Once primed, when keys are not pressed we fire

			std::string name;
			unordered_set<std::uint32_t> keys = {};
			float minDuration = 0.0f;

			// If true this event won't fire unles ONLY the keys are pressed for the entire duration
			bool exclusive = false;
			TriggerType trigger = TriggerType::Once;
			InputEventState state = InputEventState::Idle;
			BlockInputTypes blockinput = BlockInputTypes::Automatic;
	};

	struct RegisteredInputEvent {
		std::function<void(const ManagedInputEvent&)> callback = nullptr;
		std::function<bool(void)> condition = nullptr;

		RegisteredInputEvent(std::function<void(const ManagedInputEvent&)> callback, std::function<bool(void)> condition) : callback(callback) , condition(condition){

		}
	};

	using EventResult = RE::BSEventNotifyControl;

	class InputManager {
		public:
			[[nodiscard]] static InputManager& GetSingleton() noexcept;

			void ProcessEvents(InputEvent** a_event);
			std::atomic_bool Ready = false;

			std::string DebugName();

			void Init();

			static void RegisterInputEvent(std::string_view namesv, std::function<void(const ManagedInputEvent&)> callback, std::function<bool(void)> condition = nullptr);

			std::vector<ManagedInputEvent> LoadInputEvents();

			std::mutex LoadLock;

		private:
			std::unordered_map<std::string, RegisteredInputEvent> registedInputEvents;
			std::vector<ManagedInputEvent> keyTriggers;
			std::vector<GTSInputEvent> KeybindsLocalCopy; 
	};
}
