#include "Assert.hpp"
#include "CharacterController.hpp"
#include "InputComponent.hpp"
#include "IInputManager.hpp"
#include "LogLocal.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    SMOL_ACTOR(CharacterController)
    SMOL_ACTOR_END(CharacterController)

    CharacterController::CharacterController(IInputManager& manager){
        inputComponent = AddComponent<InputComponent>(manager);
        SMOL_ASSERT(inputComponent != nullptr);
    }

    CharacterController::CharacterController(SpawnContext* context){
        if(context == nullptr){
            LOG_WARN("CharacterController not initialized!");
            return;
        }

        inputComponent = AddComponent<InputComponent>(context->inputManager);
        SMOL_ASSERT(inputComponent != nullptr);
    }
}
