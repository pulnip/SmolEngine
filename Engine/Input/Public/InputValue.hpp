#pragma once

#include "Primitives.hpp"

namespace Smol
{
    enum class InputValueType: u8{
        Digital = 0,
        Axis1D = 1,
        Axis2D = 2,
        Axis3D = 3
    };

    struct InputValue{
        Vec3 raw{
            .x = 0.0f,
            .y = 0.0f,
            .z = 0.0f
        };
        InputValueType type = InputValueType::Digital;
    };
}
