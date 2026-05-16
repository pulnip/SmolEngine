#include "MyActor.hpp"
#include "CustomComponent.hpp"
#include "ActorFactory.hpp"
#include <print>

SMOL_ACTOR(MyActor)
SMOL_ACTOR_END(MyActor)

SMOL_ACTOR(SpecialActor)
    .SetFactory([](){ return std::make_unique<SpecialActor>(42); })
SMOL_ACTOR_END(SpecialActor)

MyActor::MyActor(){
    AddComponent<CustomComponent>();
}

void MyActor::OnUpdate(float dt){
    auto rb = GetComponent<Smol::RigidBody>();
    auto cc = GetComponent<CustomComponent>();

    std::println("MyActor Update: {}, {} {}", dt, Smol::u64(rb), Smol::u64(cc));
}
