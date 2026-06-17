#include <format>
#include "GameManager.hpp"
#include "CameraConfig.hpp"
#include "LineRenderer.hpp"
#include "Primitives.hpp"

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
        remainRoundTime = 0.f;

        Smol::LineRenderer* renderer = GetComponent<Smol::LineRenderer>();

        // 점수
        std::string scoreStr = std::format("{}", score);
        Smol::Vec2 scorePos = Smol::Vec2(Smol::detail::screenWidth / 2, Smol::detail::screenHeight / 2 + 30);
        renderer->DrawText(scoreStr, scorePos);

        // 쏜 화살 수
        std::string arrowCountStr = std::format("{}", arrowCount);
        Smol::Vec2 arrowCountPos = Smol::Vec2(Smol::detail::screenWidth / 2, Smol::detail::screenHeight / 2 - 30);
        renderer->DrawText(arrowCountStr, arrowCountPos);
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
    Smol::Vec2 arrowCountPos = Smol::Vec2(Smol::detail::screenWidth / 2 / 2 / 2, 30);
    renderer->DrawText(arrowCountStr, arrowCountPos);
}
