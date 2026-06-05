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
}


#define SMOL_OBJECT_BODY(Type, Parent) \
public: \
    static constexpr ::Smol::CStr StaticClassName(){ \
        return #Type; \
    } \
    virtual ::Smol::CStr GetClassName() const override{ \
        return StaticClassName(); \
    } \
    using Super = Parent; \
    virtual bool IsA(::Smol::StrView name) const override{ \
        if(name == StaticClassName()) \
            return true; \
        return Super::IsA(name); \
    } \
    static auto _SmolReflectImpl(); \
private:

#define SMOL_OBJECT(Type) \
auto Type::_SmolReflectImpl(){ \
    return ::Smol::Reflect<Type>() \
        .SetName(#Type) \
        .Inherits<::Smol::Object>()

#define SMOL_OBJECT_END(Type) \
        .Build(); \
} \
namespace{ \
    const auto Is##Type##Registered = Type::_SmolReflectImpl(); \
}
