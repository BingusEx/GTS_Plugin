#include "Hooks/Skyrim/ActorRotation.hpp"

namespace Hooks { // This hook is commented out inside hooks.cpp

	void Hook_ActorRotation::Hook(Trampoline& trampoline) { // Sadly, works on player only and seems to depend on Camera Angle. Reports only Player.

        /*static FunctionHook<float(Actor* actor)> Skyrim_GetActorRotationX(  // 36601 = 1405EDD40 (SE), AE = ???
            REL::RelocationID(36601, 36601),
            [](auto* actor) {
                float result = Skyrim_GetActorRotationX(actor);
                float transient_rotation = GetTinyRotation_X(actor);
                if (transient_rotation != 0.0f) {
                    result = transient_rotation;
                    log::info("Rotation != 0");
                }
                
                log::info("Rotation X of {} is: {}", actor->GetDisplayFullName(), result);
                
                
                return result;
            }
        );
    

    static FunctionHook<void(TESObjectREFR* ref, float x)> Skyrim_SetRefRotationX( // 19360 = 140296680 (SE)
        // Reports values properly on actor, but still does nothing.
        REL::RelocationID(19360, 19360),
        [](auto* ref, auto x) {
            //log::info("Raw Name Ref: {}", GetRawName(ref)); 
            //log::info("Pos: {}", Vector2Str(pos));
            float rotation = GetTinyRotation_X(ref);
            if (rotation != 0.0f) {
                x = rotation;
                log::info("X of {} is {}", ref->GetDisplayFullName(), x);
            }
            
            return Skyrim_SetRefRotationX(ref, x);
        });*/
    }
}



