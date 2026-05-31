#pragma once

#include "Semantics.hpp"

namespace Smol
{
    class Object{
    public:
        Object() = default;
        virtual ~Object() = default;
        SMOL_DECLARE_MOVE_ONLY(Object)
    };
}
