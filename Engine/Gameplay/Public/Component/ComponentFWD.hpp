#pragma once

#include "CoreFWD.hpp"
#include "Primitives.hpp"

namespace Smol
{
    class Component;
    using ComponentRAII = RAII<Component>;

    class InputComponent;
    class RigidBody;
    constexpr u32 NUM_BUILTIN_COMPONENTS = 2;
}
