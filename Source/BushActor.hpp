#pragma once

#include <vector>

#include "Actor.hpp"

class BushActor: public Smol::Actor{
    SMOL_ACTOR_BODY(BushActor)

public:
    BushActor();

    void OnStart() override;

    void OnUpdate(float) override;
};
