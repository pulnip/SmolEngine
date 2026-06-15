#pragma once

#include "Pawn.hpp"

namespace Smol
{
    class ColliderComponent;
    struct OverlapResult2D;
}


class ExampleActor: public Smol::Pawn{
    SMOL_ACTOR_BODY(ExampleActor, Smol::Pawn)

public:
    void OnStart() override;
    void OnUpdate(float) override;

    void PossessedBy(Smol::CharacterController&) override;

private:
    void OnMouseClicked(Smol::InputValue);
    void OnMouseMove(Smol::InputValue);

    void OnMoveStarted(Smol::InputValue);
    void OnMoveTriggered(Smol::InputValue);
    void OnMoveFinished(Smol::InputValue);

    void OnOverlapBegin(
        Smol::ColliderComponent*,
        Smol::Actor*, Smol::ColliderComponent*,
        const Smol::OverlapResult2D&
    );
    void OnOverlapStay(
        Smol::ColliderComponent*,
        Smol::Actor*, Smol::ColliderComponent*,
        const Smol::OverlapResult2D&
    );
    void OnOverlapEnd(
        Smol::ColliderComponent*,
        Smol::Actor*, Smol::ColliderComponent*
    );
};
