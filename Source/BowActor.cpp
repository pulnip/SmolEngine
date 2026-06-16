#include "BowActor.hpp"
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

}
