#pragma once

#include "Actor.hpp"
#include "ClassBuilder.hpp"

namespace Smol
{
    ObjectRAII CreateObject(StrView name);
    ActorRAII CreateActor(StrView name);
}

#define SMOL_ACTOR(Type) \
namespace{ \
    const auto _##Type##_registered = []{ \
        return Smol::Reflect<Type>() \
            .SetName(#Type) \
            .Inherits<Smol::Actor>()

#define SMOL_ACTOR_END() \
            .Build(); \
    }(); \
}
