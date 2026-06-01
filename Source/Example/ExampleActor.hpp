#pragma once

#include "ActorRegistry.hpp"

class SimpleActor: public Smol::Actor{
    SMOL_ACTOR_BODY(SimpleActor, Smol::Actor)

private:
    int hp;

public:
    SimpleActor();

    void OnUpdate(float) override;
};

class ComplexActor: public Smol::Actor{
    SMOL_ACTOR_BODY(ComplexActor, Smol::Actor)

public:
    ComplexActor(int var){}

    void OnUpdate(float) override{}
};
