#include "ExampleActor.hpp"
#include "ColliderComponent.hpp"
#include "InputAction.hpp"
#include "LineRenderer.hpp"
#include "LogLocal.hpp"
#include "MoveComponent.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"

SMOL_ACTOR(ExampleActor, Smol::Pawn)
SMOL_ACTOR_END(ExampleActor)

void ExampleActor::OnStart(){
    using namespace Smol;

    if(auto c = GetComponent<ColliderComponent>()){
        c->OnBeginOverlap(this, &ExampleActor::OnOverlapBegin);
        c->OnStayOverlap(this, &ExampleActor::OnOverlapStay);
        c->OnEndOverlap(this, &ExampleActor::OnOverlapEnd);
    }
    else{
        LOG_WARN("No ColliderComponent at ExampleActor");
    }
}

void ExampleActor::OnUpdate(float dt){
    using namespace Smol;

    // LOG_INFO("ExampleActor Update");

    if(auto c = GetComponent<LineRenderer>()){
        c->DrawText("Hello, World!", Vec2{200, 450});
    }
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

    BindAction(
        "MajorAction", TriggerEvent::Started,
        this, &ExampleActor::OnMouseClicked
    );
    BindAction(
        "MajorAction", TriggerEvent::Triggered,
        this, &ExampleActor::OnMouseMove
    );
}

void ExampleActor::OnMouseClicked(Smol::InputValue v){
    LOG_DEBUG("Mouse Clicked");
}

void ExampleActor::OnMouseMove(Smol::InputValue v){
    auto dpos = v.GetAxis2D();

    LOG_DEBUG("Mouse Move. dps = {}", dpos);
}

void ExampleActor::OnMoveStarted(Smol::InputValue v){
    using namespace Smol;

    if(auto c = GetComponent<SpriteAnimComponent>()){
        c->SetAnimation("walk");
    }
    else{
        LOG_WARN("No SpriteAnimComponent in ExampleActor");
    }

    auto dir = v.GetAxis3D();

    LOG_DEBUG("Start Move to {}", dir);

    if(auto c = GetComponent<MoveComponent>()){
        c->SetDirection(dir);
    }
    else{
        LOG_WARN("No MoveComponent in ExampleActor");
    }
}

void ExampleActor::OnMoveTriggered(Smol::InputValue v){
    using namespace Smol;

    auto dir = v.GetAxis3D();

    LOG_DEBUG("Trigger Move to {}", dir);

    /*
    if(auto c = GetComponent<LineRenderer>()){
        static std::vector<Vec3> lines1 = {
            Vec3{0, 0},
            Vec3{2, 0},
            Vec3{4, 2},
            Vec3{6, 2},
            Vec3{8, 4},
            Vec3{10, 4}
        };

        static std::vector<Vec3> lines2 = {
            Vec3{-10, -2},
            Vec3{-8, 0},
            Vec3{-6, 0},
            Vec3{-4, 2},
            Vec3{-2, 2},
            Vec3{-0, 4}
        };

        c->DrawLine(lines1, Colors::Blue);
        c->DrawLine(lines2);
    }
    else{
        LOG_WARN("No LineRenderer in ExampleActor");
    }
    */

    if(auto c = GetComponent<MoveComponent>()){
        c->SetDirection(dir);
    }
    else{
        LOG_WARN("No MoveComponent in ExampleActor");
    }
}

void ExampleActor::OnMoveFinished(Smol::InputValue v){
    using namespace Smol;

    auto dir = v.GetAxis3D();

    LOG_DEBUG("Finish Move to {}", dir);

    if(auto c = GetComponent<SpriteAnimComponent>()){
        c->SetAnimation("idle");
    }
    else{
        LOG_WARN("No SpriteAnimComponent in ExampleActor");
    }

    if(auto c = GetComponent<MoveComponent>()){
        c->SetDirection(zeros());
    }
    else{
        LOG_WARN("No MoveComponent in ExampleActor");
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
