#include <stdexcept>
#include "Actor.hpp"
#include "ActorRegistry.hpp"
#include "LogLocal.hpp"
#include "SpawnContext.hpp"
#include "World.hpp"

namespace Smol
{
    World::World() = default;
    World::~World(){
        isShutdown = true;
    }

    World::World(StrView scenePath){
        // TODO.
        throw std::runtime_error("Unimplemented");
    }

    World::World(SpawnContext& ctx){
        ctx.dom.forEach("actors", [&](const DOM::Value& node){
            auto type = node.get<Str>("type");
            auto name = node.get<Str>("name");

            if(!type || !name) return;

            if(auto it = handleMap.find(*name); it != handleMap.end()){
                LOG_WARN("Actor name \"{}\" already exists", *name);
                return;
            }

            SpawnContext spawnContext{
                .dom = node,
                .inputManager = ctx.inputManager,
                .device = ctx.device,
                .spriteRenderer = ctx.spriteRenderer,
                .world = this
            };

            auto actor = CreateActor(*type, spawnContext);
            if(actor == nullptr) [[unlikely]]{
                LOG_WARN("Actor type {} not exist", *type);
                return;
            }

            auto ptr = actor.get();
            auto handle = actors.emplace(std::move(actor));
            ptr->MarkManaged(this, handle);

            // register name with handle
            handleMap[*name] = handle;
            handleToName[handle] = *name;
        });
    }

    void World::Update(f32 deltaTime){
        for(auto& actor: actors){
            actor->OnUpdate(deltaTime);
        }

        // lazy destroy
        flushDestroy();

        for(auto& actor: actors){
            actor->SyncRenderState();
        }
    }

    Actor* World::FindActorByName(StrView name) const{
        auto it = handleMap.find(name);
        if(it == handleMap.end()){
            return nullptr;
        }

        return actors.get(it->second).get();
    }

    void World::MarkDestroy(Handle handle){
        if(!handle.IsValid()) [[unlikely]]
            return;

        SMOL_ASSERT(actors.find(handle) != nullptr);
        // double destroy is prevented by actor
        pendingDestory.push_back(handle);
    }

    void World::flushDestroy(){
        std::swap(pendingDestory, destroyScratch);

        for(auto& handle: destroyScratch){
            actors.remove(handle);

            auto it = handleToName.find(handle);
            SMOL_ASSERT(it != handleToName.end());

            handleMap.erase(it->second);
            handleToName.erase(it);
        }
        destroyScratch.clear();
    }
}
