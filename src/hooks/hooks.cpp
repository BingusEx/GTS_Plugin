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
#include "Hooks/Skyrim/Character.hpp"
#include "Hooks/Skyrim/Movement.hpp"
#include "Hooks/Skyrim/Pushback.hpp"
#include "Hooks/Skyrim/RaceMenu.hpp"
#include "Hooks/Skyrim/Stealth.hpp"
#include "Hooks/Skyrim/Impact.hpp"
#include "Hooks/Skyrim/Damage.hpp"
#include "Hooks/Skyrim/Actor.hpp"
#include "Hooks/Skyrim/Input.hpp"
#include "Hooks/Skyrim/Havok.hpp"
#include "Hooks/Skyrim/Main.hpp"
#include "Hooks/Skyrim/Sink.hpp"
#include "Hooks/Skyrim/Jump.hpp"
#include "Hooks/Skyrim/VM.hpp"

#include "Hooks/Skyrim/D3DPresent.hpp"
#include "Hooks/Controls.hpp"
#include "Hooks/Experiments.hpp"

using namespace RE;

namespace Hooks
{
	void InstallControls() {

		log::info("Applying Control Hooks...");

		Hook_Controls<ActivateHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ActivateHandler[0]));
		/*Hook_Controls<AttackBlockHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AttackBlockHandler[0]));
		Hook_Controls<AutoMoveHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_AutoMoveHandler[0]));
		Hook_Controls<MovementHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_MovementHandler[0]));
		Hook_Controls<ReadyWeaponHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ReadyWeaponHandler[0]));
		Hook_Controls<RunHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_RunHandler[0]));*/
		Hook_Controls<JumpHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_JumpHandler[0]));
		Hook_Controls<ShoutHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ShoutHandler[0]));
		Hook_Controls<SneakHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SneakHandler[0]));
		Hook_Controls<ToggleRunHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ToggleRunHandler[0]));
		/*Hook_Controls<ThirdPersonState>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_ThirdPersonState[0]));
		Hook_Controls<SprintHandler>::Hook(REL::Relocation<std::uintptr_t>(RE::VTABLE_SprintHandler[0]));
		*/
		log::info("Gts finished applying Control hooks...");
	}

	void Install()
	{
		log::info("Gts applying hooks...");

		auto& trampoline = SKSE::GetTrampoline();

		trampoline.create(512);

		Hook_MainUpdate::Hook(trampoline);
		Hook_Projectiles::Hook(); // Experimental stuff with scaling arrows and other projectiles. It works but mostly visually.
		Hook_BGSImpactManager::Hook();
		Hook_BGSImpactManager::Hook(trampoline);
		Hook_VM::Hook();
		Hook_Havok::Hook(trampoline);

		Hook_hkbBehaviorGraph::Hook();
		Hook_PlayerCharacter::Hook();
		Hook_Actor::Hook(trampoline);
		Hook_ActorEquipManager::Hook(trampoline);
		Hook_Character::Hook();
		Hook_Sinking::Hook(trampoline);
		Hook_Jumping::Hook(trampoline);
		Hook_Damage::Hook(trampoline);
		Hook_Pushback::Hook(trampoline);
		Hook_Input::Hook(trampoline);
		Hook_Renderer::Hook(trampoline);

		InstallControls();
		//if (REL::Module::IsSE()) { // Used when something is not RE'd yet for AE
			
		//}
		Hook_Stealth::Hook(trampoline);
		Hook_Movement::Hook(trampoline);
		
		Hook_HeadTracking::Hook(trampoline);
		Hook_PreventAnimations::Hook(trampoline);
		Hook_RaceMenu::Hook(trampoline);
		
		HookCameraStates();

		//Hook_MagicTarget::Hook();
		//Hook_ActorRotation::Hook(trampoline);
		//Hook_Experiments::Hook(trampoline);

		log::info("Gts finished applying hooks...");
	}
}