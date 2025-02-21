#include "Hooks/Hooks.hpp"

#include "Hooks/Skyrim/PreventAnimations.hpp"
#include "Hooks/Skyrim/ActorEquipManager.hpp"
#include "Hooks/Skyrim/hkbBehaviorGraph.hpp"
#include "Hooks/Skyrim/PlayerCharacter.hpp"
#include "Hooks/Skyrim/ActorRotation.hpp"
#include "Hooks/Skyrim/HeadTracking.hpp"
#include "Hooks/Skyrim/PlayerCamera.hpp"
#include "Hooks/Skyrim/MagicTarget.hpp"
#include "Hooks/Skyrim/Projectiles.hpp"
#include "Hooks/Skyrim/CameraState.hpp"
#include "Hooks/Skyrim/D3DPresent.hpp"
#include "Hooks/Skyrim/Character.hpp"
#include "Hooks/Skyrim/Movement.hpp"
#include "Hooks/Skyrim/Controls.hpp"
#include "Hooks/Skyrim/PushAway.hpp"
#include "Hooks/Skyrim/Pushback.hpp"
#include "Hooks/Skyrim/RaceMenu.hpp"
#include "Hooks/Skyrim/Stealth.hpp"
#include "Hooks/Skyrim/Console.hpp"
#include "Hooks/Skyrim/Impact.hpp"
#include "Hooks/Skyrim/Damage.hpp"
#include "Hooks/Skyrim/Actor.hpp"
#include "Hooks/Skyrim/Input.hpp"
#include "Hooks/Skyrim/Havok.hpp"
#include "Hooks/Skyrim/Main.hpp"
#include "Hooks/Skyrim/Sink.hpp"
#include "Hooks/Skyrim/Jump.hpp"
#include "Hooks/Skyrim/VM.hpp"

#include "Hooks/Experiments.hpp"

namespace Hooks
{
	void InstallControls() {

		log::info("Applying Control Hooks...");

		Hook_Controls<ActivateHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ActivateHandler[0]));
		Hook_Controls<JumpHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_JumpHandler[0]));
		Hook_Controls<ShoutHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ShoutHandler[0]));
		Hook_Controls<SneakHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SneakHandler[0]));
		Hook_Controls<ToggleRunHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ToggleRunHandler[0]));

		//Hook_Controls<AttackBlockHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AttackBlockHandler[0]));
		//Hook_Controls<AutoMoveHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AutoMoveHandler[0]));
		//Hook_Controls<MovementHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_MovementHandler[0]));
		//Hook_Controls<ReadyWeaponHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ReadyWeaponHandler[0]));
		//Hook_Controls<RunHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_RunHandler[0]));
		//Hook_Controls<ThirdPersonState>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ThirdPersonState[0]));
		//Hook_Controls<SprintHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SprintHandler[0]));
		
		log::info("Applied Control Hooks");
	}

	void Install(){

		log::info("Gts applying hooks...");

		auto& trampoline = SKSE::GetTrampoline();
		trampoline.create(256);

		Hook_VM::Hook();
		Hook_Character::Hook();
		Hook_Projectiles::Hook(); // Experimental stuff with scaling arrows and other projectiles. It works but mostly visually.
		Hook_BGSImpactManager::Hook();
		Hook_PlayerCharacter::Hook();
		Hook_hkbBehaviorGraph::Hook();

		Hook_MainUpdate::Hook(trampoline);
		Hook_Input::Hook(trampoline);
		Hook_Renderer::Hook(trampoline);
		Hook_BGSImpactManager::Hook(trampoline);
		Hook_Havok::Hook(trampoline);
		Hook_Actor::Hook(trampoline);
		Hook_ActorEquipManager::Hook(trampoline);
		Hook_Sinking::Hook(trampoline);
		Hook_Jumping::Hook(trampoline);
		Hook_Damage::Hook(trampoline);
		Hook_PushBack::Hook(trampoline);
		Hook_PushAway::Hook(trampoline);
		Hook_Stealth::Hook(trampoline);
		Hook_Movement::Hook(trampoline);
		Hook_HeadTracking::Hook(trampoline);
		Hook_PreventAnimations::Hook(trampoline);
		Hook_RaceMenu::Hook(trampoline);
		Hook_Console::Hook(trampoline);

		HookCameraStates();
		InstallControls();

		//if (REL::Module::IsSE()) { // Used when something is not RE'd yet for AE

		//}

		//Hook_MagicTarget::Hook();
		//Hook_ActorRotation::Hook(trampoline);
		//Hook_Experiments::Hook(trampoline);

		log::info("Finished applying hooks");
		log::info("Default Trampoline Used: {}/{} Bytes", trampoline.allocated_size(), trampoline.capacity());
		


	}
}