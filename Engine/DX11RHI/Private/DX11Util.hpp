#pragma once

#include <dxgiformat.h>
#include "RHIDefinitions.hpp"

namespace Smol
{
    DXGI_FORMAT convert(RHIPixelFormat,
        bool isShaderResource=true,
        bool isDepthTarget=false
    );
    D3D11_COMPARISON_FUNC convert(RHIComparisonFunc);

    RHIPixelFormat convert(DXGI_FORMAT);
}
