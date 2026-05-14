#include "Actor.hpp"
#include "ActorFactory.hpp"

int main(int, char*[]){
    using namespace Smol;

    auto a = CreateActor("MyActor");

    auto o = CreateObject("SpecialActor");
    a->AddComponent<Smol::RigidBody>();

    a->OnUpdate(0.0f);

    return 0;
}