#include <utility>
#include "EnumUtil.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    template<>
    CStr EnumTraits<RHIBackend>::name = "RHIBackend";

    template<>
    CStr EnumTraits<RHIBackend>::convert(RHIBackend e){
        using enum RHIBackend;

        switch(e){
    #if defined(_WIN32)
        case DirectX11: return "DirectX11";
        case DirectX12: return "DirectX12";
    #elif defined(__APPLE__)
        case Metal:     return "Metal";
    #endif
        case WebGPU:    return "WebGPU";
        default:
            std::unreachable();
        }
    }
}
