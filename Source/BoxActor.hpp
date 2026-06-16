#pragma once

#include "Actor.hpp"

class BoxActor: public Smol::Actor{
    SMOL_ACTOR_BODY(BoxActor)

public:
    BoxActor();

    void OnStart() override;

    void OnUpdate(float) override;
};
