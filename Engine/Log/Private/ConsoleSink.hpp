#pragma once

#include "Sink.hpp"

namespace Smol
{
    class ConsoleSink: public Sink{
    public:
        void Write(const LogMessage&) override;
    };
}
