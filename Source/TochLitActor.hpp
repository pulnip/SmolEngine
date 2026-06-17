#pragma once

#include "Actor.hpp"
#include "Primitives.hpp"

class TochLitActor: public Smol::Actor{
    SMOL_ACTOR_BODY(TochLitActor)

public:
    TochLitActor();

    void OnStart() override;

    void OnUpdate(float) override;

private:
    Smol::f32 accelerationGrowthRate = 80.f;

    Smol::f32 fallSpeed = 0.f;  // 현재 떨어지는 속도
    Smol::f32 fallAcceleration = 0.f;
};
