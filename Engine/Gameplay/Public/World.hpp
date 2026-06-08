#pragma once

#include <unordered_map>
#include <vector>
#include "Actor.hpp"
#include "GenericHandle.hpp"
#include "PtrUtil.hpp"
#include "Resource.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    template<Resource T>
    class ResourceManager;
    class IInputManager;
    class SpriteRenderer;

    struct EngineService{
        ResourceManager<SpriteResource>* spriteManager = nullptr;
        IInputManager* inputManager = nullptr;
        SpriteRenderer* spriteRenderer = nullptr;
    };

    namespace DOM
    {
        class Value;
    }

    class World final{
    private:
        using Handle = GenericHandle<ActorRAII>;
        SlotMap<ActorRAII> actors;
        StringHashMap<Handle> handleMap;
        std::unordered_map<Handle, Str> handleToName;
        // for runtime Actor Creation
        StringHashMap<usize> nameSeed;

        std::vector<Handle> pendingDestory;

        bool isShutdown = false;

        EngineService service;

    public:
        World();
        ~World();
        SMOL_DECLARE_MOVE_ONLY(World)

        World(EngineService service)
            : service(service){}

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

            return ptr;
        }

        // Spawn user-defined Actor for engine
        Actor* SpawnActor(StrView type, StrView name);

        void MarkDestroy(Handle);

        void Update(f32 deltaTime);

        Actor* FindActorByName(StrView name) const;
        bool IsShutdown() const noexcept{ return isShutdown; }

        // Get Engine Service
        ResourceManager<SpriteResource>* GetSpriteManager() const noexcept{
            return service.spriteManager;
        }
        IInputManager* GetInputManager() const noexcept{
            return service.inputManager;
        }
        SpriteRenderer* GetSpriteRenderer() const noexcept{
            return service.spriteRenderer;
        }

    private:
        std::vector<Handle> destroyScratch;

        void manageActor(ActorRAII&& actor, Str name);

        void flushDestroy();
    };
}
