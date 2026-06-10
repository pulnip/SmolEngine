#include "Assert.hpp"
#include "Actor.hpp"
#include "ColliderComponent.hpp"
#include "Geometry/Overlap2D.hpp"
#include "LogLocal.hpp"
#include "Object.hpp"
#include "PhysicsEngine2D.hpp"
#include "PtrUtil.hpp"
#include "World.hpp"

namespace{
    void OnEnter(
        Smol::Object* a, Smol::Object* b,
        const Smol::OverlapResult2D& result
    ){
        SMOL_ASSERT(a != nullptr);
        SMOL_ASSERT(a->IsA("ColliderComponent"));
        SMOL_ASSERT(b != nullptr);
        SMOL_ASSERT(b->IsA("ColliderComponent"));

        using namespace Smol;

        auto ca= static_cast<ColliderComponent*>(a);
        auto cb = static_cast<ColliderComponent*>(b);

        ca->NotifyBeginOverlap(cb, result);
    }

    void OnStay(
        Smol::Object* a, Smol::Object* b,
        const Smol::OverlapResult2D& result
    ){
        SMOL_ASSERT(a != nullptr);
        SMOL_ASSERT(a->IsA("ColliderComponent"));
        SMOL_ASSERT(b != nullptr);
        SMOL_ASSERT(b->IsA("ColliderComponent"));

        using namespace Smol;

        auto ca= static_cast<ColliderComponent*>(a);
        auto cb = static_cast<ColliderComponent*>(b);

        ca->NotifyStayOverlap(cb, result);
    }

    void OnExit(
        Smol::Object* a, Smol::Object* b
    ){
        SMOL_ASSERT(a != nullptr);
        SMOL_ASSERT(a->IsA("ColliderComponent"));
        SMOL_ASSERT(b != nullptr);
        SMOL_ASSERT(b->IsA("ColliderComponent"));

        using namespace Smol;

        auto ca= static_cast<ColliderComponent*>(a);
        auto cb = static_cast<ColliderComponent*>(b);

        ca->NotifyEndOverlap(cb);
    }
}

namespace Smol
{
    World::World()
        : physicsEngine(OnEnter, OnStay, OnExit)
    {}

    World::~World(){
        isShutdown = true;
    }

    World::World(EngineService service)
        : service(service)
        , physicsEngine(OnEnter, OnStay, OnExit)
    {}

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
        physicsEngine.Update();

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
