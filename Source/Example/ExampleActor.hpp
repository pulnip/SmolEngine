#pragma once

#include "ActorRegistry.hpp"
#include "Pawn.hpp"

class ExampleActor: public Smol::Pawn{
    SMOL_ACTOR_BODY(ExampleActor, Smol::Pawn)

public:
    void OnUpdate(float) override;

    void PossessedBy(Smol::CharacterController&) override;

private:
    void OnMoveStarted(Smol::InputValue);
    void OnMoveTriggered(Smol::InputValue);
    void OnMoveFinished(Smol::InputValue);
};
