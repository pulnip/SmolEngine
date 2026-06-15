#include "Assert.hpp"
#include "Actor.hpp"
#include "ColliderComponent.hpp"
#include "Geometry/Overlap2D.hpp"
#include "LogLocal.hpp"
#include "Object.hpp"
#include "PhysicsEngine2D.hpp"
#include "PtrUtil.hpp"
#include "World.hpp"

namespace Smol
{
    World::World()
        : physicsEngine(
            [this](Object* a, Object* b, const OverlapResult2D& r){ OnEnter(a, b, r); },
            [this](Object* a, Object* b, const OverlapResult2D& r){ OnStay(a, b, r); },
            [this](Object* a, Object* b){ OnExit(a, b); }
        )
    {}

    World::~World(){
        isShutdown = true;
    }

    World::World(EngineService service)
        : service(service)
        , physicsEngine(
            [this](Object* a, Object* b, const OverlapResult2D& r){ OnEnter(a, b, r); },
            [this](Object* a, Object* b, const OverlapResult2D& r){ OnStay(a, b, r); },
            [this](Object* a, Object* b){ OnExit(a, b); }
        )
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

        pendingStart.emplace_back(ptr);

        return ptr;
    }

    void World::manageActor(ActorRAII&& actor, Str name){
        auto ptr = actor.get();
        auto handle = actors.Emplace(std::move(actor));
        ptr->MarkManaged(this, handle);

        handleMap[name] = handle;
        handleToName[handle] = name;
    }

    void World::Update(f32 deltaTime){
        for(auto& actor: pendingStart){
            actor->OnStart();
        }
        pendingStart.clear();

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

        return actors.GetRef(it->second).get();
    }

    void World::MarkDestroy(Handle handle){
        if(!handle.IsValid()) [[unlikely]]
            return;

        SMOL_ASSERT(actors.Find(handle) != nullptr);
        // double destroy is prevented by actor
        pendingDestory.push_back(handle);
    }

    void World::flushDestroy(){
        std::swap(pendingDestory, destroyScratch);

        for(auto& handle: destroyScratch){
            actors.Remove(handle);

            auto it = handleToName.find(handle);
            SMOL_ASSERT(it != handleToName.end());

            handleMap.erase(it->second);
            handleToName.erase(it);
        }
        destroyScratch.clear();
    }

    void World::OnEnter(
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

    void World::OnStay(
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

    void World::OnExit(
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
