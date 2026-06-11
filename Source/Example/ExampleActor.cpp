#include "ExampleActor.hpp"
#include "ColliderComponent.hpp"
#include "InputAction.hpp"
#include "LineRenderer.hpp"
#include "LogLocal.hpp"
#include "MoveComponent.hpp"
#include "Primitives.hpp"

SMOL_ACTOR(ExampleActor, Smol::Pawn)
SMOL_ACTOR_END(ExampleActor)

void ExampleActor::OnStart(){
    auto collider = GetComponent<Smol::ColliderComponent>();

    collider->OnBeginOverlap(this, &ExampleActor::OnOverlapBegin);
    collider->OnStayOverlap(this, &ExampleActor::OnOverlapStay);
    collider->OnEndOverlap(this, &ExampleActor::OnOverlapEnd);
}

void ExampleActor::OnUpdate(float dt){
    // LOG_INFO("ExampleActor Update");
}

void ExampleActor::PossessedBy(Smol::CharacterController& controller){
    using namespace Smol;
    using enum TriggerEvent;

    Super::PossessedBy(controller);

    BindAction(
        "Move", TriggerEvent::Started,
        this, &ExampleActor::OnMoveStarted
    );
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

    LOG_DEBUG("Start Move to {}", dir);

    auto moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(dir);
    }
}

void ExampleActor::OnMoveTriggered(Smol::InputValue v){
    auto dir = v.GetAxis3D();

    using namespace Smol;

    auto renderer = GetComponent<Smol::LineRenderer>();
    std::vector<Vec3> lines1 = {
        Vec3{0, 0},
        Vec3{2, 0},
        Vec3{4, 2},
        Vec3{6, 2},
        Vec3{8, 4},
        Vec3{10, 4}
    };

    std::vector<Vec3> lines2 = {
        Vec3{-10, -2},
        Vec3{-8, 0},
        Vec3{-6, 0},
        Vec3{-4, 2},
        Vec3{-2, 2},
        Vec3{-0, 4}
    };

    renderer->DrawLine(lines1, Colors::Blue);
    renderer->DrawLine(lines2);

    LOG_DEBUG("Trigger Move to {}", dir);

    auto moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(dir);
    }
}

void ExampleActor::OnMoveFinished(Smol::InputValue v){
    auto dir = v.GetAxis3D();

    LOG_DEBUG("Finish Move to {}", dir);

    auto moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(Smol::zeros());
    }
}

void ExampleActor::OnOverlapBegin(
    Smol::ColliderComponent*,
    Smol::Actor*, Smol::ColliderComponent*,
    const Smol::OverlapResult2D& result
){
    LOG_INFO(" Overlap Start. {}", result.contactPoint);
}

void ExampleActor::OnOverlapStay(
    Smol::ColliderComponent*,
    Smol::Actor*, Smol::ColliderComponent*,
    const Smol::OverlapResult2D& result
){
    // LOG_INFO("Overlap Stay. {}", result.contactPoint);
}

void ExampleActor::OnOverlapEnd(
    Smol::ColliderComponent*,
    Smol::Actor*, Smol::ColliderComponent*
){
    LOG_INFO("Overlap End.");
}
