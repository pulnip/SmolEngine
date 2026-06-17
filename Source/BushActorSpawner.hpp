#pragma once

#include <vector>

#include "Actor.hpp"

class BushActor;

class BushActorSpawner: public Smol::Actor{
    SMOL_ACTOR_BODY(BushActorSpawner)

public:
    BushActorSpawner();

    void OnStart() override;

    void OnUpdate(float) override;

    void ClearCachedSpawnActor(Smol::Actor* actor);
private:
    BushActor* cachedSpawnActor = nullptr;

    float spawnDelay = 1.f;
    float elapsedSpawnTime = 0.f;
};
