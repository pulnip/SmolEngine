#pragma once

#include "ActorFWD.hpp"
#include "Primitives.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    ActorRAII CreateActor(StrView type, const SpawnContext& context);
}
