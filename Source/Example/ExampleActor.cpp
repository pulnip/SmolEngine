#include "ExampleActor.hpp"
#include "InputAction.hpp"
#include "LogLocal.hpp"

SMOL_ACTOR(ExampleActor)
SMOL_ACTOR_END(ExampleActor)

void ExampleActor::OnUpdate(float dt){
    LOG_INFO("ExampleActor Update");
}

void ExampleActor::PossessedBy(Smol::CharacterController& controller){
    using namespace Smol;
    using enum TriggerEvent;

    Super::PossessedBy(controller);

    BindAction(
        "Move", TriggerEvent::Triggered,
        this, &ExampleActor::OnMoveTriggered
    );
    BindAction(
        "Move", TriggerEvent::Finished,
        this, &ExampleActor::OnMoveFinished
    );
}


void ExampleActor::OnMoveStarted(Smol::InputValue v){
    auto dir = v.GetAxis3D();

    LOG_INFO("Start Move to {}", dir);
}

void ExampleActor::OnMoveTriggered(Smol::InputValue v){
    auto dir = v.GetAxis3D();

    LOG_INFO("Trigger Move to {}", dir);
}

void ExampleActor::OnMoveFinished(Smol::InputValue v){
    auto dir = v.GetAxis3D();

    LOG_INFO("Finish Move to {}", dir);
}
