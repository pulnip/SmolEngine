#pragma once

#include "CoreFWD.hpp"
#include "Semantics.hpp"
#include "LogDefinitions.hpp"

namespace Smol
{
    struct LogMessage;

    class Sink{
    public:
        SMOL_DECLARE_INTERFACE(Sink)

        virtual void Write(const LogMessage&) = 0;
    };

    void AddSink(RAII<Sink>);
}
