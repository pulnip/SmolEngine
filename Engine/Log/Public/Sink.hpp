#pragma once

#include "Semantics.hpp"
#include "LogDefinitions.hpp"

namespace Smol
{
    class Sink{
    public:
        SMOL_DECLARE_INTERFACE(Sink)

        virtual void Write(const LogMessage&) = 0;
    };
}
