#pragma once

#include "ActorFactory.hpp"

class MyActor: public Smol::Actor{
    SMOL_CLASS_BODY(MyActor)

private:
    int hp;

public:
    MyActor();

    void OnUpdate(float) override;
};

class SpecialActor: public Smol::Actor{
    SMOL_CLASS_BODY(SpecialActor)

public:
    SpecialActor(int var){}

    void OnUpdate(float){}
};
