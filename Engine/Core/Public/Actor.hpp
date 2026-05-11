#pragma once

#include <array>
#include <unordered_map>
#include <vector>
#include "ActorFactory.hpp"
#include "Component.hpp"
#include "Primitives.hpp"
#include "Object.hpp"

namespace Smol
{
    struct Transform{
        Vec3 position;
        Vec4 rotation;
        Vec3 scale;
    };

    template<typename T>
    consteval u32 GetComponentTypeIndex(){
        if constexpr(std::is_same_v<T, RigidBody>) return 0;
        return NUM_BUILTIN_COMPONENTS;
    }

    template<typename T>
    consteval bool IsBuiltinComponent(){
        if constexpr(std::is_same_v<T, RigidBody>) return true;
        return false;
    }

    class Actor: public Object{
    private:
        Transform transform;
        std::array<ComponentRAII, NUM_BUILTIN_COMPONENTS> builtinComponents;
        std::unordered_map<Component::TypeID, ComponentRAII> userdefinedComponents;

        std::vector<ActorRAII> children;

    public:
        SMOL_DECLARE_INTERFACE(Actor)

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

        virtual void OnUpdate(float) = 0;
    };
}

#define SMOL_ACTOR(Type) \
namespace{ \
    const auto _##Type##_registered = []{ \
        return Smol::ActorFactory::Get().Register(#Type, []{ \
            return std::make_unique<Type>(); \
        }); \
    }(); \
}
