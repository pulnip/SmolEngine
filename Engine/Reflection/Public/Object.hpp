#pragma once

#include "ClassRegistry.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class Object{
    public:
        Object() = default;
        virtual ~Object() = default;
        SMOL_DECLARE_MOVE_ONLY(Object)

        static constexpr CStr StaticClassName(){
            return "Object";
        }
        virtual CStr GetClassName() const{
            return StaticClassName();
        }

        virtual bool IsA(StrView name) const{
            return name == StaticClassName();
        }
        static auto _SmolReflectImpl();
    };

    template<typename T>
        requires std::is_base_of_v<Object, T>
    ClassBuilder<T> Reflect(){
        return ClassBuilder<T>();
    }
}

#define SMOL_MACRO_DISPATCHER_FOR_2_ARGS(_1, _2, NAME, ...) NAME

// Object Body macro
#define SMOL_OBJECT_BODY_IMPL(TYPE, PARENT) \
public: \
    static constexpr ::Smol::CStr StaticClassName(){ \
        return #TYPE; \
    } \
    virtual ::Smol::CStr GetClassName() const override{ \
        return StaticClassName(); \
    } \
    using Super = PARENT; \
    virtual bool IsA(::Smol::StrView name) const override{ \
        if(name == StaticClassName()) \
            return true; \
        return Super::IsA(name); \
    } \
    static auto _SmolReflectImpl(); \
private:

#define SMOL_OBJECT_BODY_DIRECT(TYPE) \
    SMOL_OBJECT_BODY_IMPL(TYPE, ::Smol::Object)

#define SMOL_OBJECT_BODY_HELPER(_1, _2, NAME, ...) NAME
#define SMOL_OBJECT_BODY(...) \
    SMOL_OBJECT_BODY_HELPER( \
        __VA_ARGS__, \
        SMOL_OBJECT_BODY_IMPL, \
        SMOL_OBJECT_BODY_DIRECT \
    )(__VA_ARGS__)

// Object Registration start macro
#define SMOL_OBJECT_IMPL(TYPE, PARENT) \
auto TYPE::_SmolReflectImpl(){ \
    return ::Smol::Reflect<TYPE>() \
        .SetName(#TYPE) \
        .Inherits<PARENT>()

#define SMOL_OBJECT_DIRECT(TYPE) \
    SMOL_OBJECT_IMPL(TYPE, ::Smol::Object)

#define SMOL_OBJECT(...) \
    SMOL_MACRO_DISPATCHER_FOR_2_ARGS( \
        __VA_ARGS__, \
        SMOL_OBJECT_IMPL, \
        SMOL_OBJECT_DIRECT \
    )(__VA_ARGS__)

// Object Registration end macro
#define SMOL_OBJECT_END(TYPE) \
        .Build(); \
} \
namespace{ \
    const auto Is##TYPE##Registered = TYPE::_SmolReflectImpl(); \
}
