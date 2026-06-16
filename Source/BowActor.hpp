#pragma once

#include "Actor.hpp"

class BowActor: public Smol::Actor{
    SMOL_ACTOR_BODY(BowActor)

public:
    BowActor();

    void OnStart() override;
};
