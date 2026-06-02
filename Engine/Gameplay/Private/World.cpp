#include "Actor.hpp"
#include "World.hpp"
#include "ActorRegistry.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    World::World(StrView scenePath){
        SpawnContext spawnContext{};

        auto a = CreateActor("SimpleActor", spawnContext);
        a->AddComponent<Smol::RigidBody>();

        auto o = CreateActor("ComplexActor", spawnContext);

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
