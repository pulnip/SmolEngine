#pragma once

#include "ActorRegistry.hpp"
#include "InputValue.hpp"
#include "Pawn.hpp"

namespace Smol
{
    class TestPawn: public Pawn{
        SMOL_CLASS_BODY(TestPawn)

    public:
        TestPawn() = default;
        ~TestPawn() = default;
        SMOL_DECLARE_MOVE_ONLY(TestPawn)

        void PossessedBy(CharacterController&) override;

    private:
        void OnMoveStarted(InputValue);
        void OnMoveTriggered(InputValue);
        void OnMoveFinished(InputValue);

        void OnJumpStarted(InputValue);
        void OnJumpTriggered(InputValue);
        void OnJumpFinished(InputValue);
    };
}
