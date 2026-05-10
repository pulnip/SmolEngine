#pragma once

#include "Semantics.hpp"

namespace Smol
{
    class MainLoop{
    public:
        SMOL_DECLARE_INTERFACE(MainLoop)

        virtual void initialize(){}
        virtual bool update(float deltaTime, float totalTime) = 0;
        virtual void finalize(){}
    };
}
