#pragma once

#include "Actor.hpp"
#include "ClassBuilder.hpp"

namespace Smol
{
    ObjectRAII CreateObject(StrView name);
    ActorRAII CreateActor(StrView name);
}

#define SMOL_CLASS_BODY(Type) \
public: \
    static auto _SmolReflectImpl(); \
private: \

#define SMOL_ACTOR(Type) \
inline auto Type::_SmolReflectImpl(){ \
    return ::Smol::Reflect<Type>() \
        .SetName(#Type) \
        .Inherits<::Smol::Actor>()

#define SMOL_ACTOR_END(Type) \
        .Build(); \
} \
namespace{ \
    const auto _##Type##Registered = Type::_SmolReflectImpl(); \
}
