#pragma once

#include <unordered_map>
#include <vector>
#include "Actor.hpp"
#include "GenericHandle.hpp"
#include "PhysicsEngine2D.hpp"
#include "Resource.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"
#include "StringUtil.hpp"
#include "Timer.hpp"

namespace Smol
{
    template<Resource T>
    class ResourceManager;
    class IInputManager;
    class ShapeRenderer;
    class SpriteRenderer;

    struct EngineService{
        ResourceManager<SpriteResource>* spriteManager = nullptr;
        IInputManager* inputManager = nullptr;
        SpriteRenderer* spriteRenderer = nullptr;
        ShapeRenderer* shapeRenderer = nullptr;
    };

    namespace DOM
    {
        class Value;
    }

    struct OverlapResult2D;

    class World final{
    private:
        using Handle = GenericHandle<ActorRAII>;
        SlotMap<ActorRAII> actors;
        StringHashMap<Handle> handleMap;
        std::unordered_map<Handle, Str> handleToName;
        // for runtime Actor Creation
        StringHashMap<usize> nameSeed;

        std::vector<Actor*> pendingStart;
        std::vector<Actor*> startScratch;

        std::vector<Handle> pendingDestory;

        bool isShutdown = false;

        EngineService service;

        PhysicsEngine2D physicsEngine;

        bool debugState = false;

        Timer timer;

    public:
        World();
        ~World();
        SMOL_DECLARE_PINNED(World)

        World(EngineService service);

        template<std::derived_from<Actor> T>
        T* SpawnActor(Str name = {}){
            if(auto ptr = FindActorByName(name)){
                return nullptr;
            }

            if(name.empty()){
                auto prefix = T::StaticClassName();

                auto it = nameSeed.find(prefix);
                if(it == nameSeed.end()){
                    nameSeed[prefix] = 0;
                }

                name = std::format("{}_{}", prefix, nameSeed[prefix]++);
            }

            auto actor = std::make_unique<T>();
            auto ptr = actor.get();
            manageActor(std::move(actor), Str(name));

            pendingStart.emplace_back(ptr);

            return ptr;
        }

        // Spawn user-defined Actor for engine
        Actor* SpawnActor(StrView type, StrView name);

        void Initialize();
        void Update(f32 deltaTime);

        Actor* FindActorByName(StrView name) const;
        Actor* TryGetActor(Handle handle) const noexcept;

        bool IsShutdown() const noexcept{ return isShutdown; }

        // Get Engine Service
        ResourceManager<SpriteResource>* GetSpriteManager() const noexcept{
            return service.spriteManager;
        }
        IInputManager* GetInputManager() const noexcept{
            return service.inputManager;
        }
        ShapeRenderer* GetShapeRenderer() const noexcept{
            return service.shapeRenderer;
        }
        SpriteRenderer* GetSpriteRenderer() const noexcept{
            return service.spriteRenderer;
        }
        PhysicsEngine2D& GetPhysics() noexcept{
            return physicsEngine;
        }

        void SetDebugState(bool isDebugging) noexcept{
            debugState = isDebugging;
        }
        bool GetDebugState() noexcept{
            return debugState;
        }

        void SetTimerPaused(bool paused = true) noexcept;
        void SetTimerScale(f32 scale) noexcept;

    private:
        friend class Actor;

        // Call Order
        // 1. Actor::Destroy()
        // 2. World::MarkDestroy()
        // 3. Actor invalidates...
        //     (1) its own handle
        //     (2) pointer to world
        void MarkDestroy(Handle);

    private:
        std::vector<Handle> destroyScratch;

        void manageActor(ActorRAII&& actor, Str name);

        void flushDestroy();

        void OnEnter(Object*, Object*, const OverlapResult2D&);
        void OnStay(Object*, Object*, const OverlapResult2D&);
        void OnExit(Object*, Object*);

    private:
    };
}
