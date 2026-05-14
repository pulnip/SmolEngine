#pragma once

#include "Actor.hpp"

class MyActor: public Smol::Actor{
public:
    int hp;

public:
    MyActor();

    void OnUpdate(float) override;
};

class SpecialActor: public Smol::Actor{
public:
    SpecialActor(int var){}

    void OnUpdate(float){}
};
