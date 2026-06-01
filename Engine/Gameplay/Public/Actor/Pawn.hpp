#pragma once

#include "Actor.hpp"
#include "ActorRegistry.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class CharacterController;

    class Pawn: public Actor{
        SMOL_CLASS_BODY(Pawn)

    public:
        Pawn() = default;
        ~Pawn() = default;
        SMOL_DECLARE_MOVE_ONLY(Pawn)

        virtual void PossessedBy(CharacterController&){}
    };
}
