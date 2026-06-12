#include "PlayerCharacter.hpp"
#include "CharacterController.hpp"
#include "InputAction.hpp"
#include "MoveComponent.hpp"
#include "LineRenderer.hpp"
#include "BowComponent.hpp"
#include "Primitives.hpp"
#include "LogGame.hpp"

SMOL_ACTOR(PlayerCharacter, Smol::Pawn)
SMOL_ACTOR_END(PlayerCharacter)

void PlayerCharacter::PossessedBy(Smol::CharacterController& controller){
    Super::PossessedBy(controller);

    // 이동 입력에 대한 콜백 함수 바인딩
    BindAction("Move", Smol::TriggerEvent::Triggered, this, &PlayerCharacter::OnMove);
    BindAction("Move", Smol::TriggerEvent::Finished, this, &PlayerCharacter::EndMove);

    //TODO. 점프
    //BindAction("Jump", Smol::TriggerEvent::Started, this, &PlayerCharacter::OnJump);

    //TODO. Content/Input/Example.input.toml에 action 추가하기
    BindAction("Jump", Smol::TriggerEvent::Triggered, this, &PlayerCharacter::OnBowAim);
    BindAction("OnBowAim", Smol::TriggerEvent::Finished, this, &PlayerCharacter::OnBowRelease);
}

void PlayerCharacter::OnUpdate(float dt){

}

void PlayerCharacter::OnMove(Smol::InputValue v){
    // 입력 방향으로 이동 방향 설정

    Smol::Vec3 dir = v.GetAxis3D();

    // LOG_INFO("OnMove to {}", dir);

    Smol::MoveComponent* moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(dir);
    }
}

void PlayerCharacter::EndMove(Smol::InputValue v){
    // 이동 방향 기본값으로 초기화
    Smol::MoveComponent* moveComponent = GetComponent<Smol::MoveComponent>();
    if(moveComponent != nullptr){
        moveComponent->SetDirection(Smol::zeros());
    }
}

void PlayerCharacter::OnJump(Smol::InputValue v){
    LOG_INFO("OnJump");

}

void PlayerCharacter::OnBowAim(Smol::InputValue v){
    LOG_INFO("OnBowAim");

    BowComponent* bowComp = GetComponent<BowComponent>();
    if (nullptr == bowComp)
    {
        LOG_WARN("Game", "nullptr");
        return;
    }

    //TODO. 마우스 위치의 방향으로 변경 예정
    Smol::Vec3 direction = Smol::Vec3(1.f, 0.f, 0.f);
    std::vector<Smol::Vec3> drawTrajectory = bowComp->BuildTrajectory(direction);

    auto renderer = GetComponent<Smol::LineRenderer>();

     renderer->DrawLine(drawTrajectory);
}

void PlayerCharacter::OnBowRelease(Smol::InputValue v){
    LOG_INFO("OnBowRelease");

    BowComponent* bowComp = GetComponent<BowComponent>();
    if (nullptr == bowComp)
    {
        LOG_WARN("Game", "nullptr");
        return;
    }

    Smol::Vec3 direction = Smol::Vec3(1.f, 0.f, 0.f);
    bowComp->EndAim(direction);
}

