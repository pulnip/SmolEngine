#include <utility>
#include "MetalUtil.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    MTL::PixelFormat convertPixelFormat(RHIPixelFormat format){
        using enum RHIPixelFormat;
        using namespace MTL;

        switch(format){
        case Unknown:           return PixelFormatInvalid;
        // 8-bit formats
        case R8_UNORM:          return PixelFormatR8Unorm;
        case R8_SNORM:          return PixelFormatR8Snorm;
        case R8_UINT:           return PixelFormatR8Uint;
        case R8_SINT:           return PixelFormatR8Sint;
        // 16-bit formats
        case R16_UNORM:         return PixelFormatR16Unorm;
        case R16_SNORM:         return PixelFormatR16Snorm;
        case R16_UINT:          return PixelFormatR16Uint;
        case R16_SINT:          return PixelFormatR16Sint;
        case R16_FLOAT:         return PixelFormatR16Float;

        case RG8_UNORM:         return PixelFormatRG8Unorm;
        case RG8_SNORM:         return PixelFormatRG8Snorm;
        case RG8_UINT:          return PixelFormatRG8Uint;
        case RG8_SINT:          return PixelFormatRG8Sint;
        // 32-bit formats
        case R32_UINT:          return PixelFormatR32Uint;
        case R32_SINT:          return PixelFormatR32Sint;
        case R32_FLOAT:         return PixelFormatR32Float;

        case RG16_UNORM:        return PixelFormatRG16Unorm;
        case RG16_SNORM:        return PixelFormatRG16Snorm;
        case RG16_UINT:         return PixelFormatRG16Uint;
        case RG16_SINT:         return PixelFormatRG16Sint;
        case RG16_FLOAT:        return PixelFormatRG16Float;

        case RGBA8_UNORM:       return PixelFormatRGBA8Unorm;
        case RGBA8_UNORM_SRGB:  return PixelFormatRGBA8Unorm_sRGB;
        case RGBA8_SNORM:       return PixelFormatRGBA8Snorm;
        case RGBA8_UINT:        return PixelFormatRGBA8Uint;
        case RGBA8_SINT:        return PixelFormatRGBA8Sint;

        case BGRA8_UNORM:       return PixelFormatBGRA8Unorm;
        case BGRA8_UNORM_SRGB:  return PixelFormatBGRA8Unorm_sRGB;

        // 64-bit formats
        case RG32_UINT:         return PixelFormatRG32Uint;
        case RG32_SINT:         return PixelFormatRG32Sint;
        case RG32_FLOAT:        return PixelFormatRG32Float;

        case RGBA16_UNORM:      return PixelFormatRGBA16Unorm;
        case RGBA16_SNORM:      return PixelFormatRGBA16Snorm;
        case RGBA16_UINT:       return PixelFormatRGBA16Uint;
        case RGBA16_SINT:       return PixelFormatRGBA16Sint;
        case RGBA16_FLOAT:      return PixelFormatRGBA16Float;

        // 128-bit formats
        case RGBA32_UINT:       return PixelFormatRGBA32Uint;
        case RGBA32_SINT:       return PixelFormatRGBA32Sint;
        case RGBA32_FLOAT:      return PixelFormatRGBA32Float;

        // Depth/stencil formats
        case D16_UNORM:         return PixelFormatDepth16Unorm;
        case D24_UNORM_S8_UINT: return PixelFormatDepth24Unorm_Stencil8;
        case D32_FLOAT:         return PixelFormatDepth32Float;
        case D32_FLOAT_S8_UINT: return PixelFormatDepth32Float_Stencil8;
        default:
            std::unreachable();
        }
    }

    MTL::CompareFunction convert(RHIComparisonFunc func){
        using enum RHIComparisonFunc;
        using namespace MTL;

        switch(func){
        case Never:        return CompareFunctionNever;
        case Less:         return CompareFunctionLess;
        case Equal:        return CompareFunctionEqual;
        case LessEqual:    return CompareFunctionLessEqual;
        case Greater:      return CompareFunctionGreater;
        case NotEqual:     return CompareFunctionNotEqual;
        case GreaterEqual: return CompareFunctionGreaterEqual;
        case Always:       return CompareFunctionAlways;
        default:
            std::unreachable();
        }
    }
}
