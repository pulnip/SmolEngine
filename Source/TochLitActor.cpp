#include "TochLitActor.hpp"
#include "ElementalComponent.hpp"
#include "ColliderComponent.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "LogGame.hpp"
#include "FireActor.hpp"
#include "World.hpp"
#include <cmath>
#include <random>

SMOL_ACTOR(TochLitActor)
SMOL_ACTOR_END(TochLitActor)

TochLitActor::TochLitActor(){
}

void TochLitActor::OnStart(){
    // AddComponent<Smol::SpriteComponent>();
    // Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    // spriteComp->OnAttach("Torch");

    AddComponent<ElementalComponent>();

    ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
    elementalComp->OnFire = [this](Actor* IgnitedActor){
        // 붙어 있는 불이 없을 경우
        FireActor* fire = GetWorld()->SpawnActor<FireActor>();
        if (fire == nullptr){
            LOG_WARN("", "nullptr");
            return;
        }

        // 더이상 불을 생성하지 않도록 한다
        ElementalComponent* elementalComp = GetComponent<ElementalComponent>();
        elementalComp->OnFire = nullptr;

        fire->AttachTo(this, false);
        fire->GetTransform().position.y = 1.f;
        fire->GetTransform().scale = Smol::Vec2(0.5f, 0.5f);
    };
    elementalComp->InitProperty(30.f, 1.f, 9999999.f);

    GetTransform().scale = Smol::Vec2(2, 2);
}

float RandomRange(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

void TochLitActor::OnUpdate(float dt)
{
    float topY = 15.f;
    float bottomY = -15.f;

    fallAcceleration += accelerationGrowthRate * dt;
    fallSpeed += fallAcceleration * dt;

    GetTransform().position.y -= fallSpeed * dt;

    if (GetTransform().position.y <= bottomY)
    {
        GetTransform().position.y = topY;

        fallSpeed = 0.f;
        fallAcceleration = 0.f;

        // 랜덤한 값 설정
        accelerationGrowthRate = RandomRange(10.f, 80.f);
        GetTransform().position.x = RandomRange(-10.f, 5.f);
    }
}
