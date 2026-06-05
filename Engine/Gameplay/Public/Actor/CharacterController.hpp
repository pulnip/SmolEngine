#pragma once

#include "Actor.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputComponent;
    class IInputManager;

    // Notice! Lifetime of CharacterController should be longer than Possessed Actor
    class CharacterController: public Actor{
        SMOL_ACTOR_BODY(CharacterController, Actor)

    public:
        CharacterController() = default;
        virtual ~CharacterController() = default;
        SMOL_DECLARE_MOVE_ONLY_NOEXCEPT(CharacterController)
    };
}
