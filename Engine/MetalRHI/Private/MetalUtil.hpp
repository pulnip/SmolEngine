#pragma once

#include <Metal/MTLPixelFormat.hpp>
#include <Metal/MTLDepthStencil.hpp>
#include "RHIDefinitions.hpp"

namespace Smol
{
    MTL::PixelFormat convertPixelFormat(RHIPixelFormat);
    MTL::CompareFunction convert(RHIComparisonFunc);
}
