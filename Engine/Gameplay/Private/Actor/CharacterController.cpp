#include "Assert.hpp"
#include "CharacterController.hpp"
#include "InputComponent.hpp"
#include "IInputManager.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    SMOL_ACTOR(CharacterController)
    SMOL_ACTOR_END(CharacterController)

    CharacterController::CharacterController(IInputManager& manager){
        inputComponent = AddComponent<InputComponent>(manager);
        SMOL_ASSERT(inputComponent != nullptr);
    }

    CharacterController::CharacterController(SpawnContext& context){
        inputComponent = AddComponent<InputComponent>(context.inputManager);
        SMOL_ASSERT(inputComponent != nullptr);
    }
}
