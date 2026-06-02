#include "ExampleActor.hpp"
#include "ExampleComponent.hpp"
#include <print>

SMOL_ACTOR(SimpleActor)
SMOL_ACTOR_END(SimpleActor)

SMOL_ACTOR(ComplexActor)
    .SetFactory([](Smol::SpawnContext&){
        return std::make_unique<ComplexActor>(42);
    })
SMOL_ACTOR_END(ComplexActor)

SimpleActor::SimpleActor(){
    AddComponent<SimpleComponent>();
}

void SimpleActor::OnUpdate(float dt){
    void* rb = GetComponent<Smol::RigidBody>();
    void* cc = GetComponent<SimpleComponent>();

    std::println("SimpleActor Update: {}, {} {}", dt, rb, cc);
}
