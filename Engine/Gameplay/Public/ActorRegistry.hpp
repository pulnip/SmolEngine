#pragma once

#include "Actor.hpp"
#include "ClassRegistry.hpp"

namespace Smol
{
    ActorRAII CreateActor(StrView name, SpawnContext& context);
}

#define SMOL_ACTOR_BODY(Type, Parent) \
    SMOL_OBJECT_BODY(Type, Parent)

#define SMOL_ACTOR(Type) \
    SMOL_OBJECT(Type) \
        .Inherits<::Smol::Actor>()

#define SMOL_ACTOR_END(Type) \
    SMOL_OBJECT_END(Type)
