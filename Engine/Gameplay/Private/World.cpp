#include "Actor.hpp"
#include "World.hpp"
#include "ActorRegistry.hpp"

namespace Smol
{
    World::World(StrView scenePath){
        auto a = CreateActor("SimpleActor");
        a->AddComponent<Smol::RigidBody>();

        auto o = CreateActor("ComplexActor");

        actors.emplace_back(std::move(a));
        actors.emplace_back(std::move(o));
    }

    World::~World(){

    }

    void World::Update(f32 deltaTime){
        for(auto& actor: actors){
            actor->OnUpdate(deltaTime);
        }
    }

    Actor* World::FindActorByName(StrView name) const{
        auto it = indexByName.find(name);
        if(it == indexByName.end()){
            return nullptr;
        }

        return actors[it->second].get();
    }
}
