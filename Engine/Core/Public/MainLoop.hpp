#pragma once

#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class MainLoop{
    public:
        SMOL_DECLARE_INTERFACE(MainLoop)

        virtual void initialize(){}
        virtual bool update(f32 deltaTime, f32 totalTime) = 0;
        virtual void finalize(){}
    };
}
