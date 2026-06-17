#include <format>
#include "GameManager.hpp"
#include "Actor.hpp"
#include "CameraConfig.hpp"
#include "LineRenderer.hpp"
#include "Primitives.hpp"
#include "World.hpp"

SMOL_ACTOR(GameManager)
SMOL_ACTOR_END(GameManager)

GameManager::GameManager(){
}

void GameManager::OnStart(){
    AddComponent<Smol::LineRenderer>();
}

void GameManager::OnUpdate(float dt){
    remainRoundTime -= dt;

    // 남은 시간이 있을 경우
    if (remainRoundTime > 0.f)
    {
        Playing();
    }
    // 남은 시간이 없을 경우
    else {
        bIsPlaying = false;

        GetWorld()->SetTimerPaused(true);
        GetWorld()->SetTimerScale(0.f);

        remainRoundTime = 0.f;

        Smol::LineRenderer* renderer = GetComponent<Smol::LineRenderer>();

        // 점수
        std::string scoreStr = std::format("불태운 덤불 수 : {}", score);
        Smol::Vec2 scorePos = Smol::Vec2(Smol::detail::screenWidth / 2 - 75, Smol::detail::screenHeight / 2 - 60);
        renderer->DrawText(scoreStr, scorePos);

        // 쏜 화살 수
        std::string arrowCountStr = std::format("발사한 화살 수 : {}", arrowCount);
        Smol::Vec2 arrowCountPos = Smol::Vec2(Smol::detail::screenWidth / 2 - 75, Smol::detail::screenHeight / 2 + 0);
        renderer->DrawText(arrowCountStr, arrowCountPos);

        // 명중 비율
        std::string hitRateStr = "명중 비율 : 0";
        if (arrowCount != 0)
        {
            Smol::f32 hitRate = score / (Smol::f32)arrowCount;
            hitRateStr = std::format("명중 비율 : {:.2f}", hitRate);
        }

        Smol::Vec2 hitRatePos = Smol::Vec2(Smol::detail::screenWidth / 2 - 75, Smol::detail::screenHeight / 2 + 60);
        renderer->DrawText(hitRateStr, hitRatePos);

        Smol::Actor* findActor = GetWorld()->FindActorByName("UI_EndGame");
        if (findActor == nullptr)
        {
            return;
        }
        Smol::Vec2 scale(15, 20);
        findActor->GetTransform().scale = scale;
    }
}

void GameManager::IncreaseScore(Smol::i32 amount){
    score += amount;
}

void GameManager::IncreaseArrowCount(Smol::i32 amount){
    arrowCount += amount;
}

void GameManager::Playing(){
    Smol::LineRenderer* renderer = GetComponent<Smol::LineRenderer>();

    renderer->DrawText("남은 시간", Smol::Vec2(Smol::detail::screenWidth / 2 - 27, 20));

    std::string remainRoundTimeStr = std::format("{:.0f}", remainRoundTime);
    Smol::Vec2 remainRoundTimePos = Smol::Vec2(Smol::detail::screenWidth / 2, 40);
    renderer->DrawText(remainRoundTimeStr, remainRoundTimePos);

    // 점수
    std::string scoreStr = std::format("{}", score);
    Smol::Vec2 scorePos = Smol::Vec2(Smol::detail::screenWidth / 2 / 2, 30);
    renderer->DrawText(scoreStr, scorePos);

    // 쏜 화살 수
    std::string arrowCountStr = std::format("{}", arrowCount);
    Smol::Vec2 arrowCountPos = Smol::Vec2(83, 30);
    renderer->DrawText(arrowCountStr, arrowCountPos);

    Smol::Actor* findActor = GetWorld()->FindActorByName("UI_EndGame");
    if (findActor == nullptr)
    {
        return;
    }
    Smol::Vec2 scale(0, 0);
    findActor->GetTransform().scale = scale;
}
