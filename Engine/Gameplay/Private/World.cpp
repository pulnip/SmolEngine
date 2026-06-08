#include "Actor.hpp"
#include "LogLocal.hpp"
#include "PtrUtil.hpp"
#include "World.hpp"

namespace Smol
{
    World::World() = default;
    World::~World(){
        isShutdown = true;
    }

    Actor* World::SpawnActor(StrView type, StrView name){
        if(auto ptr = FindActorByName(name)){
            LOG_WARN("Actor name \"{}\" already exists", name);
            return nullptr;
        }

        auto object = ClassRegistry::Create(type);
        auto actor = uniqueCast<Actor>(object);
        if(actor == nullptr) [[unlikely]]{
            LOG_WARN("Actor type {} not exist", type);
            return nullptr;
        }

        auto ptr = actor.get();
        manageActor(std::move(actor), Str(name));

        return ptr;
    }

    void World::manageActor(ActorRAII&& actor, Str name){
        auto ptr = actor.get();
        auto handle = actors.emplace(std::move(actor));
        ptr->MarkManaged(this, handle);

        handleMap[name] = handle;
        handleToName[handle] = name;
    }

    void World::Update(f32 deltaTime){
        for(auto& actor: actors){
            actor->Update(deltaTime);
        }

        // lazy destroy
        flushDestroy();
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
