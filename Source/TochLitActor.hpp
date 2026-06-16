#pragma once

#include "Actor.hpp"

class TochLitActor: public Smol::Actor{
    SMOL_ACTOR_BODY(TochLitActor)

public:
    TochLitActor();

    void OnStart() override;

    void OnUpdate(float) override;
};
