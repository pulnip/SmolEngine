#pragma once

#include "RHIFWD.hpp"

namespace Smol
{
    class Renderer{
    private:
        RHIDevice& device;

    public:
        Renderer(RHIDevice&);
    };
}
