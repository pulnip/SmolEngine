#pragma once

#include <vector>

#include "Actor.hpp"

class BushActor: public Smol::Actor{
    SMOL_ACTOR_BODY(BushActor)

public:
    BushActor();

    void OnStart() override;

    void OnUpdate(float) override;

    void OnDestroy() override;

    std::function<void(Smol::Actor*)> OnDestroyed;

    Smol::f32 startX = 0.f;

private:
    Smol::f32 elapsedMoveTime = 0.f;
    Smol::f32 moveRange = 4.f;
    Smol::f32 moveSpeed = 2.f;
};
