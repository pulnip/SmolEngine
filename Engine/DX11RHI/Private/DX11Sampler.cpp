#include <utility>
#include <d3d11.h>
#include "DX11Sampler.hpp"
#include "DX11Util.hpp"
#include "RHIDefinitions.hpp"

namespace{
    auto convertAddressMode(Smol::RHIAddressMode mode){
        using enum Smol::RHIAddressMode;

        switch(mode){
        case Wrap  : return D3D11_TEXTURE_ADDRESS_WRAP;
        case Clamp : return D3D11_TEXTURE_ADDRESS_CLAMP;
        case Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
        case Border: return D3D11_TEXTURE_ADDRESS_BORDER;
        default:
            std::unreachable();
        }
    }

    auto convertFilter(
        Smol::RHIFilter min, Smol::RHIFilter mag, Smol::RHIFilter mip,
        bool anisotropy, bool comparison
    ){
        using enum Smol::RHIFilter;

        if(anisotropy)
            return comparison ?
                D3D11_FILTER_COMPARISON_ANISOTROPIC :
                D3D11_FILTER_ANISOTROPIC;

        UINT flags = 0;

        if(mip == Linear) flags |= 0x1;
        if(mag == Linear) flags |= 0x4;
        if(min == Linear) flags |= 0x10;
        if(comparison)               flags |= 0x80;

        return static_cast<D3D11_FILTER>(flags);
    }
}

namespace Smol
{
    DX11Sampler::DX11Sampler(
        Device& device,
        const RHISamplerState& desc
    ){
        D3D11_SAMPLER_DESC samplerDesc{
            .Filter = convertFilter(
                desc.minFilter, desc.magFilter, desc.mipFilter,
                desc.maxAnisotropy > 1,
                desc.compareFunc != RHIComparisonFunc::Never
            ),
            .AddressU = convertAddressMode(desc.addressU),
            .AddressV = convertAddressMode(desc.addressV),
            .AddressW = convertAddressMode(desc.addressW),
            .MipLODBias = desc.mipLODBias,
            .MaxAnisotropy = desc.maxAnisotropy,
            .ComparisonFunc = convert(desc.compareFunc),
            .BorderColor = {
                desc.borderColor[0], desc.borderColor[1],
                desc.borderColor[2], desc.borderColor[3]
            },
            .MinLOD = desc.minLOD,
            .MaxLOD = desc.maxLOD
        };
        if(FAILED(device.CreateSamplerState(
            &samplerDesc,
            &sampler
        ))){
            throw std::runtime_error("Failed to create DX11 Sampler");
        }
    }

    DX11Sampler::~DX11Sampler() = default;
}
