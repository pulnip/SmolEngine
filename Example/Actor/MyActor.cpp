#include "MyActor.hpp"
#include "CustomComponent.hpp"
#include <print>

SMOL_ACTOR(MyActor)

MyActor::MyActor(){
    AddComponent<CustomComponent>();
}

void MyActor::OnUpdate(float dt){
    auto rb = GetComponent<Smol::RigidBody>();
    auto cc = GetComponent<CustomComponent>();

    std::println("MyActor Update: {}, {} {}", dt, Smol::u64(rb), Smol::u64(cc));
}
