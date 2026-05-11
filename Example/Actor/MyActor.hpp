#pragma once

#include "Actor.hpp"

class MyActor: public Smol::Actor{
public:
    MyActor();

    void OnUpdate(float) override;
};
