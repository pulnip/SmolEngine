#pragma once

#include <type_traits>
#include "ComponentFWD.hpp"
#include "Object.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class Component: public Object{
    public:
        using TypeID = const void*;

    public:
        Component() = default;
        virtual ~Component() = default;
        SMOL_DECLARE_MOVE_ONLY(Component)

        virtual TypeID GetTypeID() const = 0;
    };

    template<typename Derived>
    class TypedComponent: public Component{
    private:
        inline static u8 typeIDStorage = 0;

    public:
        TypedComponent() = default;
        virtual ~TypedComponent() = default;
        SMOL_DECLARE_MOVE_ONLY(TypedComponent)

        static constexpr TypeID GetStaticTypeID(){ return &typeIDStorage; }
        TypeID GetTypeID() const override final{ return &typeIDStorage; }
    };

    struct Transform{
        Vec3 position;
        Vec4 rotation;
        Vec3 scale;
    };

    template<typename T>
    consteval u32 GetComponentTypeIndex(){
        if constexpr(std::is_same_v<T, InputComponent>) return 0;
        else if constexpr(std::is_same_v<T, RigidBody>) return 1;
        return NUM_BUILTIN_COMPONENTS;
    }

    template<typename T>
    consteval bool IsBuiltinComponent(){
        if constexpr(std::is_same_v<T, InputComponent>) return true;
        else if constexpr(std::is_same_v<T, RigidBody>) return true;
        return false;
    }
}
