#include <utility>
#include <Metal/Metal.hpp>
#include "MetalSampler.hpp"
#include "MetalUtil.hpp"
#include "RHIDefinitions.hpp"

namespace{
    auto convert(Smol::RHIAddressMode mode){
        using namespace Smol;
        using enum RHIAddressMode;
        using namespace MTL;

        switch(mode){
        case Wrap  : return SamplerAddressModeRepeat;
        case Clamp : return SamplerAddressModeClampToEdge;
        case Mirror: return SamplerAddressModeMirrorRepeat;
        case Border: return SamplerAddressModeClampToBorderColor;
        default:
            std::unreachable();
        }
    }

    auto convertMinMagFilter(Smol::RHIFilter filter){
        using namespace Smol;
        using enum RHIFilter;
        using namespace MTL;

        switch(filter){
        case Nearest: return SamplerMinMagFilterNearest;
        case Linear:  return SamplerMinMagFilterLinear;
        default:
            std::unreachable();
        }
    }

    auto convertMipFilter(Smol::RHIFilter filter){
        using namespace Smol;
        using enum RHIFilter;
        using namespace MTL;

        switch(filter){
        case Nearest: return SamplerMipFilterNearest;
        case Linear:  return SamplerMipFilterLinear;
        default:
            std::unreachable();
        }
    }
}

namespace Smol
{
    MetalSampler::MetalSampler(
        MTL::Device& device,
        const RHISamplerState& desc
    ){
        auto samplerDesc = MTL::SamplerDescriptor::alloc()->init();
        samplerDesc->setMinFilter(convertMinMagFilter(desc.minFilter));
        samplerDesc->setMagFilter(convertMinMagFilter(desc.magFilter));
        samplerDesc->setMipFilter(convertMipFilter(desc.magFilter));
        samplerDesc->setSAddressMode(::convert(desc.addressU));
        samplerDesc->setTAddressMode(::convert(desc.addressV));
        samplerDesc->setRAddressMode(::convert(desc.addressW));
        samplerDesc->setLodMinClamp(desc.minLOD);
        samplerDesc->setLodMaxClamp(desc.maxLOD);
        samplerDesc->setMaxAnisotropy(desc.maxAnisotropy);
        samplerDesc->setCompareFunction(convert(desc.compareFunc));

        sampler = device.newSamplerState(samplerDesc);
        samplerDesc->release();
    }

    MetalSampler::~MetalSampler(){
        sampler->release();
    }
}
