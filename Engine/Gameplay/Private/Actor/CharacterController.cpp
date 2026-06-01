#include "Assert.hpp"
#include "CharacterController.hpp"
#include "InputComponent.hpp"

namespace Smol
{
    SMOL_ACTOR(CharacterController)
    SMOL_ACTOR_END(CharacterController)

    void CharacterController::Init(ServiceLocator& locator){
        inputComponent = AddComponent<InputComponent>(locator.inputManager);
        SMOL_ASSERT(inputComponent != nullptr);
    }
}
