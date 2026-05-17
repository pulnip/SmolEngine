#pragma once

#include "RHIFrameScope.hpp"

namespace Smol
{
    class DX11FrameScope final: public RHIFrameScope{
    public:
        DX11FrameScope() = default;
        ~DX11FrameScope() = default;
    };
}
