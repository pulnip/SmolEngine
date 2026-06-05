#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include "ActorFWD.hpp"
#include "Component.hpp"
#include "GenericHandle.hpp"
#include "Object.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

#define SMOL_ACTOR_BODY(Type, Parent) \
    SMOL_OBJECT_BODY(Type, Parent)

#define SMOL_ACTOR(Type, Parent) \
    SMOL_OBJECT(Type) \
        .Inherits<Parent>()

#define SMOL_ACTOR_END(Type) \
    SMOL_OBJECT_END(Type)

namespace Smol
{
    class World;

    class Actor: public Object{
        SMOL_ACTOR_BODY(Actor, Object)

    private:
        std::array<ComponentRAII, NUM_BUILTIN_COMPONENTS> builtinComponents;
        std::unordered_map<Component::TypeID, ComponentRAII> userdefinedComponents;

        std::vector<ActorRAII> children;

    protected:
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

        void Update(f32);

        template<typename T, class... Args>
            requires (!IsBuiltinComponent<T>())
        T* AddComponent(Args&&... args){
            constexpr auto typeID = TypedComponent<T>::GetStaticTypeID();
            auto c = std::make_unique<T>(std::forward<Args>(args)...);

            auto [it, ret] = userdefinedComponents.try_emplace(typeID, std::move(c));
            if(!ret){
                return nullptr;
            }

            return static_cast<T*>(it->second.get());
        }
        template<typename T, class... Args>
            requires (IsBuiltinComponent<T>())
        T* AddComponent(Args&&...);

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
        T* GetComponent();

        void Destroy();

    protected:
        virtual void OnUpdate(f32){}

    private:
        void updateComponents(f32);

    private:
        friend class World;

        // World ptr and Handle is valid if
        // 1. Actor is managed by World
        // 2. And Also Actor not destroyed at World
        World* world = nullptr;
        using Handle = GenericHandle<ActorRAII>;
        Handle handle = Handle::InvalidHandle();

        void MarkManaged(World*, Handle);

    protected:
        // for skip
        bool IsWorldShutdown() const noexcept;
    };
}
