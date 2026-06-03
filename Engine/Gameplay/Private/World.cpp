#include <stdexcept>
#include "Actor.hpp"
#include "ActorRegistry.hpp"
#include "LogLocal.hpp"
#include "SpawnContext.hpp"
#include "World.hpp"

namespace Smol
{
    World::World() = default;
    World::~World() = default;

    World::World(StrView scenePath){
        // TODO.
        throw std::runtime_error("Unimplemented");
    }

    World::World(SpawnContext& ctx, std::span<const Str> actorNames){
        for(const auto& actorName: actorNames){
            SpawnContext spawnContext{
                .inputManager = ctx.inputManager
            };

            auto actor = CreateActor(actorName, spawnContext);

            auto [_, ret] = indexByName.try_emplace(
                actorName,
                actors.size()
            );

            if(!ret){
                LOG_WARN("Actor {} not exists.", actorName);
                continue;
            }

            actors.emplace_back(std::move(actor));
        }
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
