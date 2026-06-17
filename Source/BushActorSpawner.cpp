#include "BushActorSpawner.hpp"
#include "Actor.hpp"
#include "BushActor.hpp"
#include "LogGame.hpp"
#include "World.hpp"

SMOL_ACTOR(BushActorSpawner)
SMOL_ACTOR_END(BushActorSpawner)

BushActorSpawner::BushActorSpawner()
{

}

void BushActorSpawner::OnStart(){

}

void BushActorSpawner::OnUpdate(float dt){
    if (cachedSpawnActor != nullptr)
    {
        return;
    }

    elapsedSpawnTime += dt;
    if (spawnDelay > elapsedSpawnTime)
    {
        return;
    }

    elapsedSpawnTime = 0.f;

    cachedSpawnActor = GetWorld()->SpawnActor<BushActor>();
    cachedSpawnActor->GetTransform() = GetTransform();
    cachedSpawnActor->startX = GetTransform().position.x;

    cachedSpawnActor->OnDestroyed = [this] (Smol::Actor* actor) {
        if (cachedSpawnActor == actor)
        {
            cachedSpawnActor = nullptr;
        }
    };
}
