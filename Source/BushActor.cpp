#include "BushActor.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "ColliderComponent.hpp"
#include "ElementalComponent.hpp"
#include "LogGame.hpp"
#include "World.hpp"
#include "FireActor.hpp"

SMOL_ACTOR(BushActor, Smol::Actor)
SMOL_ACTOR_END(BushActor)

BushActor::BushActor()
{

}

void BushActor::OnStart(){
    AddComponent<Smol::SpriteComponent>();
    Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach("Bush");

    AddComponent<Smol::ColliderComponent>()->OnAttach();
    Smol::ColliderComponent* colliderComp = GetComponent<Smol::ColliderComponent>();

    colliderComp->SetLayer(0b1);
    colliderComp->SetMask(0b0);

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
        fire->GetTransform().scale = Smol::Vec2(0.4f, 0.2f);
    };
    elementalComp->InitProperty(30.f, 50.f, 3.f);

    GetTransform().scale = Smol::Vec2(5, 5);
}

void BushActor::OnUpdate(float dt){

}
