#pragma once

#include "Actor.hpp"
#include "ActorRegistry.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputComponent;
    class IInputManager;

    // Notice! Lifetime of CharacterController should be longer than Possessed Actor
    class CharacterController: public Actor{
        SMOL_ACTOR_BODY(CharacterController, Actor)

    private:
        InputComponent* inputComponent = nullptr;

    public:
        CharacterController() = default;
        virtual ~CharacterController() = default;
        CharacterController(CharacterController&& other)
            : inputComponent(other.inputComponent)
        {
            other.inputComponent = nullptr;
        }
        CharacterController& operator=(CharacterController&& other){
            inputComponent = other.inputComponent;
            other.inputComponent = nullptr;

            return *this;
        }
        SMOL_DECLARE_NON_COPYABLE(CharacterController)

        // explicitly type test
        CharacterController(IInputManager&);
        // Called from Deserialization
        CharacterController(SpawnContext*);

        InputComponent& GetInputComponent() const noexcept{ return *inputComponent; }
    };
}
