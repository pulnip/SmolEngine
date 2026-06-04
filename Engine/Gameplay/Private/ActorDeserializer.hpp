#pragma once

#include "Actor.hpp"
#include "Primitives.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    ActorRAII CreateActor(StrView type, SpawnContext& context);
}
