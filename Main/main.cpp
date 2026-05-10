#include "ActorFactory.hpp"
#include "Actor.hpp"

int main(int, char*[]){
    auto a = Smol::ActorFactory::Get().Create("MyActor");

    a->update(0.0f);

    return 0;
}