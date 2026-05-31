#pragma once

#include "Actor.hpp"
#include "ActorFactory.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class CharacterController: public Actor{
        SMOL_CLASS_BODY(CharacterController)

    public:
        CharacterController() = default;
        virtual ~CharacterController() = default;
        SMOL_DECLARE_MOVE_ONLY(CharacterController)
    };
}
