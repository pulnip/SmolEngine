#pragma once

#include "ActorRegistry.hpp"

class SimpleActor: public Smol::Actor{
    SMOL_CLASS_BODY(SimpleActor)

private:
    int hp;

public:
    SimpleActor();

    void OnUpdate(float) override;
};

class ComplexActor: public Smol::Actor{
    SMOL_CLASS_BODY(ComplexActor)

public:
    ComplexActor(int var){}

    void OnUpdate(float){}
};
