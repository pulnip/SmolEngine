#include "MyActor.hpp"
#include <print>

SMOL_ACTOR(MyActor)

void MyActor::update(float dt){
    std::println("MyActor Update: {}", dt);
}
