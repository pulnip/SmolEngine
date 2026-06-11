#pragma once

#include <type_traits>
#include "Object.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class Actor;

    class Component: public Object{
        SMOL_OBJECT_BODY(Component)

    public:
        using TypeID = const void*;

    protected:
        Actor* owner = nullptr;

    public:
        Component() = default;
        virtual ~Component() = default;
        SMOL_DECLARE_PINNED(Component)

        virtual TypeID GetTypeID() const = 0;

        virtual void Update(f32){}

        void MarkManaged(Actor* owner) noexcept;

        Actor* GetOwner() const noexcept{ return owner; }
    };

    template<typename Derived>
    class TypedComponent: public Component{
    private:
        inline static u8 typeIDStorage = 0;

    public:
        TypedComponent() = default;
        virtual ~TypedComponent() = default;
        SMOL_DECLARE_PINNED(TypedComponent)

        static constexpr TypeID GetStaticTypeID(){ return &typeIDStorage; }
        TypeID GetTypeID() const override final{ return &typeIDStorage; }
    };

    using ComponentRAII = RAII<Component>;

    class InputComponent;
    class LineRenderer;
    class MoveComponent;
    class Rigidbody;
    class SpriteAnimComponent;
    class SpriteComponent;

    template<typename T>
    consteval auto GetComponentTypeIndex(){
        u32 index = 0;

    #define COUNT_COMPONENT(TYPE) \
        if constexpr(std::is_same_v<T, TYPE>){ \
            return index; \
        } ++index;

        COUNT_COMPONENT(InputComponent)
        COUNT_COMPONENT(LineRenderer)
        COUNT_COMPONENT(MoveComponent)
        COUNT_COMPONENT(Rigidbody)
        COUNT_COMPONENT(SpriteAnimComponent)
        COUNT_COMPONENT(SpriteComponent)

    #undef COUNT_COMPONENT

        return index;
    }
    constexpr auto NUM_BUILTIN_COMPONENTS = GetComponentTypeIndex<void>();

    template<typename T>
    consteval bool IsBuiltinComponent(){
        return GetComponentTypeIndex<T>() != NUM_BUILTIN_COMPONENTS;
    }

    template<typename T>
    concept BuiltinComponent = IsBuiltinComponent<T>();
}

// Component Body macro
#define SMOL_COMPONENT_BODY_IMPL(TYPE, PARENT) \
    SMOL_OBJECT_BODY(TYPE, PARENT)

#define SMOL_COMPONENT_BODY_DIRECT(TYPE) \
    SMOL_COMPONENT_BODY_IMPL(TYPE, ::Smol::Component)

#define SMOL_COMPONENT_BODY_HELPER(_1, _2, NAME, ...) NAME
#define SMOL_COMPONENT_BODY(...) \
    SMOL_COMPONENT_BODY_HELPER( \
        __VA_ARGS__, \
        SMOL_COMPONENT_BODY_IMPL, \
        SMOL_COMPONENT_BODY_DIRECT, \
    )(__VA_ARGS__)

// Component Registeration start macro
#define SMOL_COMPONENT_IMPL(TYPE, PARENT) \
    SMOL_OBJECT(TYPE, PARENT)

#define SMOL_COMPONENT_DIRECT(TYPE) \
    SMOL_COMPONENT_IMPL(TYPE, ::Smol::Component)

#define SMOL_COMPONENT(...) \
    SMOL_MACRO_DISPATCHER_FOR_2_ARGS( \
        __VA_ARGS__, \
        SMOL_COMPONENT_IMPL, \
        SMOL_COMPONENT_DIRECT \
    )(__VA_ARGS__)

// Component Registeration end macro
#define SMOL_COMPONENT_END(TYPE) \
    SMOL_OBJECT_END(TYPE)
