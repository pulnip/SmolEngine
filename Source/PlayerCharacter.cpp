#include "PlayerCharacter.hpp"
#include "BowActor.hpp"
#include "CharacterController.hpp"
#include "Geometry/GeoUtil.hpp"
#include "IInputManager.hpp"
#include "InputAction.hpp"
#include "LinearAlgebra.hpp"
#include "MoveComponent.hpp"
#include "LineRenderer.hpp"
#include "BowComponent.hpp"
#include "Primitives.hpp"
#include "LogGame.hpp"
#include "SpriteAnimComponent.hpp"
#include "World.hpp"

// Temporal Header
#include "CameraConfig.hpp"

SMOL_ACTOR(PlayerCharacter, Smol::Pawn)
SMOL_ACTOR_END(PlayerCharacter)

void PlayerCharacter::PossessedBy(Smol::CharacterController& controller){
    Super::PossessedBy(controller);

    // 이동 입력에 대한 콜백 함수 바인딩
    BindAction("Move", Smol::TriggerEvent::Started, this, &PlayerCharacter::StartMove);
    BindAction("Move", Smol::TriggerEvent::Triggered, this, &PlayerCharacter::OnMove);
    BindAction("Move", Smol::TriggerEvent::Finished, this, &PlayerCharacter::EndMove);

    //TODO. 점프
    BindAction("Jump", Smol::TriggerEvent::Started, this, &PlayerCharacter::OnJump);

    BindAction("MajorAction", Smol::TriggerEvent::Started, this, &PlayerCharacter::OnBowShoot);
    BindAction("AssistAction", Smol::TriggerEvent::Started, this, &PlayerCharacter::StartBowAim);
    BindAction("AssistAction", Smol::TriggerEvent::Triggered, this, &PlayerCharacter::OnBowAim);
    BindAction("AssistAction", Smol::TriggerEvent::Finished, this, &PlayerCharacter::EndBowAim);
}

void PlayerCharacter::OnStart(){
    startPos = GetTransform().position;
}

void PlayerCharacter::OnUpdate(float dt){

}

void PlayerCharacter::StartMove(Smol::InputValue v){
    BowComponent* bowComp = GetComponent<BowComponent>();
    bool bIsAiming = bowComp->GetIsAiming();
    if (bIsAiming){
        return;
    }

    Smol::SpriteAnimComponent* spriteAnimComp = GetComponent<Smol::SpriteAnimComponent>();
    spriteAnimComp->SetAnimation("walk");
}

void PlayerCharacter::OnMove(Smol::InputValue v)
{
    BowComponent* bowComp = GetComponent<BowComponent>();
    bool bIsAiming = bowComp->GetIsAiming();

    if (bIsAiming)
    {
        return;
    }

    float inputX = v.GetAxis2D().x;

    float moveRange = 2.f;
    float minX = startPos.x - moveRange;
    float maxX = startPos.x + moveRange;

    float currentX = GetTransform().position.x;

    // 왼쪽 범위 밖으로 가려는 입력이면 막기
    if (currentX <= minX && inputX < 0.f)
    {
        inputX = 0.f;
    }

    // 오른쪽 범위 밖으로 가려는 입력이면 막기
    if (currentX >= maxX && inputX > 0.f)
    {
        inputX = 0.f;
    }

    Smol::Vec2 dir = Smol::Vec2(inputX, 0.f);

    Smol::MoveComponent* moveComponent = GetComponent<Smol::MoveComponent>();

    if (moveComponent != nullptr)
    {
        moveComponent->SetDirection(dir);
    }
}

void PlayerCharacter::EndMove(Smol::InputValue v){
    BowComponent* bowComp = GetComponent<BowComponent>();
    bool bIsAiming = bowComp->GetIsAiming();
    if (bIsAiming){
        return;
    }

    // 이동 방향 기본값으로 초기화
    Smol::MoveComponent* moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(Smol::zeros());
    }

    Smol::SpriteAnimComponent* spriteAnimComp = GetComponent<Smol::SpriteAnimComponent>();
    spriteAnimComp->SetAnimation("idle");
}

void PlayerCharacter::OnJump(Smol::InputValue v){
    //LOG_INFO("OnJump");

}

void PlayerCharacter::StartBowAim(Smol::InputValue v){
    BowComponent* bowComp = GetComponent<BowComponent>();
    bowComp->SetIsAiming(true);

    Smol::SpriteAnimComponent* spriteAnimComp = GetComponent<Smol::SpriteAnimComponent>();
    spriteAnimComp->SetAnimation("aim");

    // 이동 방향 기본값으로 초기화
    Smol::MoveComponent* moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(Smol::zeros());
    }
}

void PlayerCharacter::OnBowAim(Smol::InputValue v){
    //LOG_INFO("OnBowAim");

    BowComponent* bowComp = GetComponent<BowComponent>();
    if (nullptr == bowComp)
    {
        LOG_WARN("Game", "nullptr");
        return;
    }

    Smol::Vec3 direction = Smol::Vec3(1.f, 0.f, 0.f);
    if (const Smol::IInputManager* inputManager = GetWorld()->GetInputManager())
    {
        const Smol::Vec2 mousePos = Smol::ScreenToWorld2D(inputManager->GetMousePos());
        const Smol::Vec3 actorPos = GetTransform().position;

        const Smol::Vec3 toMouse = mousePos - Smol::Vec2(actorPos.x, actorPos.y);

        direction = Smol::normalize(toMouse);
    }

    bowComp->SetBowDirection(direction);
    std::vector<Smol::Vec3> drawTrajectory = bowComp->BuildTrajectory(direction);

    auto renderer = GetComponent<Smol::LineRenderer>();

    renderer->DrawLine(drawTrajectory);
}

void PlayerCharacter::EndBowAim(Smol::InputValue v){
    BowComponent* bowComp = GetComponent<BowComponent>();
    if (nullptr == bowComp)
    {
        LOG_WARN("Game", "nullptr");
        return;
    }

    bowComp->SetIsAiming(false);

    Smol::SpriteAnimComponent* spriteAnimComp = GetComponent<Smol::SpriteAnimComponent>();
    spriteAnimComp->SetAnimation("idle");
}

void PlayerCharacter::OnBowShoot(Smol::InputValue v){
    //LOG_INFO("OnBowShoot");

    BowComponent* bowComp = GetComponent<BowComponent>();
    if (nullptr == bowComp)
    {
        LOG_WARN("Game", "nullptr");
        return;
    }

    Smol::Vec3 direction = Smol::Vec3(1.f, 0.f, 0.f);
    if (const Smol::IInputManager* inputManager = GetWorld()->GetInputManager())
    {
        const Smol::Vec2 mousePos = Smol::ScreenToWorld2D(inputManager->GetMousePos());
        const Smol::Vec3 actorPos = GetTransform().position;

        const Smol::Vec3 toMouse = mousePos - Smol::Vec2(actorPos.x, actorPos.y);

        direction = Smol::normalize(toMouse);
    }

    bowComp->Shoot(direction);
}
