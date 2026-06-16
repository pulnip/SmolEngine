#include "BoxActor.hpp"
#include "ElementalComponent.hpp"
#include "ColliderComponent.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "LogGame.hpp"
#include "FireActor.hpp"
#include "World.hpp"

SMOL_ACTOR(BoxActor)
SMOL_ACTOR_END(BoxActor)

BoxActor::BoxActor(){
}

void BoxActor::OnStart(){
    AddComponent<Smol::SpriteComponent>();
    Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach("MetalBox");

    GetTransform().scale = Smol::Vec2(3, 3);
}

void BoxActor::OnUpdate(float dt){

}
