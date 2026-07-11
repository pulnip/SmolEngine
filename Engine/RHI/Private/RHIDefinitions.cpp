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
        case DirectX11: return "DirectX11";
        case Metal:     return "Metal";
        default:
            std::unreachable();
        }
    }
}
