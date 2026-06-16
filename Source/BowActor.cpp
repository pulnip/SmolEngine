#include "BowActor.hpp"
#include "Primitives.hpp"
#include "SpriteAnimComponent.hpp"
#include "SpriteComponent.hpp"
#include "LogGame.hpp"

SMOL_ACTOR(BowActor)
SMOL_ACTOR_END(BowActor)

BowActor::BowActor(){

}

void BowActor::OnStart(){
    AddComponent<Smol::SpriteComponent>();
    Smol::SpriteComponent* spriteComp = GetComponent<Smol::SpriteComponent>();
    spriteComp->OnAttach("Pixel_Hero-Bow");

    GetTransform().position = Smol::Vec2(0.35f, 0.35f);
    GetTransform().scale = Smol::Vec2(7, 7);
}
