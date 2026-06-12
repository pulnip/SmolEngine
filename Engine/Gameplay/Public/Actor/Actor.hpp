#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include "ActorFWD.hpp"
#include "ClassRegistry.hpp"
#include "Component.hpp"
#include "GenericHandle.hpp"
#include "Object.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class World;

    class Actor: public Object{
        SMOL_OBJECT_BODY(Actor)

    private:
        std::array<ComponentRAII, NUM_BUILTIN_COMPONENTS> builtinComponents;
        std::unordered_map<Component::TypeID, ComponentRAII> userdefinedComponents;

        std::vector<ActorRAII> children;

    private:
        Transform transform{
            .position = zeros(),
            .rotation = unitQuat(),
            .scale = ones()
        };

    public:
        Actor() = default;
        virtual ~Actor() = default;
        Actor(Actor&&) noexcept;
        Actor& operator=(Actor&&) noexcept;
        SMOL_DECLARE_NON_COPYABLE(Actor)

        virtual void OnStart(){}

        void Update(f32);

        // Built-in Component, Called both engine and user
        template<BuiltinComponent T>
        T* AddComponent(){
            constexpr auto index = GetComponentTypeIndex<T>();
            auto c = std::make_unique<T>();
            c->MarkManaged(this);

            builtinComponents[index] = std::move(c);
            return static_cast<T*>(builtinComponents[index].get());
        }

        // User-defined Component, but Called by engine
        Component* AddComponent(StrView type);

        // User-defined Component, but Called by user
        template<typename T, class... Args>
            requires (!IsBuiltinComponent<T>())
        T* AddComponent(Args&&... args){
            constexpr auto typeID = TypedComponent<T>::GetStaticTypeID();
            auto c = std::make_unique<T>(std::forward<Args>(args)...);
            c->MarkManaged(this);

            auto [it, ret] = userdefinedComponents.try_emplace(typeID, std::move(c));
            if(!ret){
                return nullptr;
            }

            return static_cast<T*>(it->second.get());
        }

        World* GetWorld() const noexcept{ return world; }
        auto& GetTransform(this auto& self) noexcept{ return self.transform; }

        template<typename T>
            requires (!IsBuiltinComponent<T>())
        T* GetComponent(){
            constexpr auto typeID = TypedComponent<T>::GetStaticTypeID();

            // TODO. use unordered_map later
            auto it = userdefinedComponents.find(typeID);
            if(it == userdefinedComponents.end()){
                return nullptr;
            }

            return static_cast<T*>(it->second.get());
        }
        template<typename T>
            requires (IsBuiltinComponent<T>())
        T* GetComponent(){
            constexpr auto index = GetComponentTypeIndex<T>();
            return static_cast<T*>(builtinComponents[index].get());
        }

        void Destroy();

        // Check if Actor is Marked as Destroyed
        // or Actor is NOT Managed by World
        bool IsValid() const noexcept;

        // for skip
        bool IsWorldShutdown() const noexcept;

    protected:
        virtual void OnUpdate(f32){}

    private:
        void updateComponents(f32);

    private:
        friend class World;

        World* world = nullptr;
        // Handle for prevent double free
        // That means, Actor is managed by world
        // If Actor not destroyed at World, Handle is valid.
        using Handle = GenericHandle<ActorRAII>;
        Handle handle = Handle::InvalidHandle();

        void MarkManaged(World* world, Handle handle);
    };
}

// Actor Body macro
#define SMOL_ACTOR_BODY_IMPL(TYPE, PARENT) \
    SMOL_OBJECT_BODY(TYPE, PARENT)

#define SMOL_ACTOR_BODY_DIRECT(TYPE) \
    SMOL_ACTOR_BODY_IMPL(TYPE, ::Smol::Actor)

#define SMOL_ACTOR_BODY_HELPER(_1, _2, NAME, ...) NAME
#define SMOL_ACTOR_BODY(...) \
    SMOL_ACTOR_BODY_HELPER( \
        __VA_ARGS__, \
        SMOL_ACTOR_BODY_IMPL, \
        SMOL_ACTOR_BODY_DIRECT, \
    )(__VA_ARGS__)

// Actor Registeration start macro
#define SMOL_ACTOR_IMPL(TYPE, PARENT) \
    SMOL_OBJECT(TYPE, PARENT)

#define SMOL_ACTOR_DIRECT(TYPE) \
    SMOL_ACTOR_IMPL(TYPE, ::Smol::Actor)

#define SMOL_ACTOR(...) \
    SMOL_MACRO_DISPATCHER_FOR_2_ARGS( \
        __VA_ARGS__, \
        SMOL_ACTOR_IMPL, \
        SMOL_ACTOR_DIRECT \
    )(__VA_ARGS__)

// Actor Registeration end macro
#define SMOL_ACTOR_END(TYPE) \
    SMOL_OBJECT_END(TYPE)
