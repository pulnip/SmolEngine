#include "TochLitActor.hpp"
#include "ElementalComponent.hpp"
#include "ColliderComponent.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "LogGame.hpp"
#include "FireActor.hpp"
#include "World.hpp"

SMOL_ACTOR(TochLitActor)
SMOL_ACTOR_END(TochLitActor)

TochLitActor::TochLitActor(){
}

void TochLitActor::OnStart(){
    AddComponent<Smol::SpriteComponent>();
    Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach("Torch");

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
        fire->GetTransform().position.y = GetTransform().position.y + 2.f;
        fire->GetTransform().scale = Smol::Vec2(0.5f, 0.5f);
    };
    elementalComp->InitProperty(30.f, 1.f, 9999999.f);

    GetTransform().scale = Smol::Vec2(2, 2);
}

void TochLitActor::OnUpdate(float dt){

}
