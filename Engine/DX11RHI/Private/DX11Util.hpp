#pragma once

#include <format>
#include <stdexcept>
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

    Str HResultToString(HRESULT hr);
}

#define CHECK_HRESULT(expr, msg) \
    do{ \
        if(const HRESULT hr = (expr); FAILED(hr)) [[unlikely]]{ \
            throw std::runtime_error(std::format( \
                "{}: {}", msg, ::Smol::HResultToString(hr) \
            )); \
        } \
    } while(false)
