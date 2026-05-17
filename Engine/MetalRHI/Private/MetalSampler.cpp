#include <utility>
#include <Metal/Metal.hpp>
#include "MetalSampler.hpp"
#include "MetalUtil.hpp"

namespace Smol
{
    inline auto convert(RHIAddressMode mode){
        using namespace MTL;

        switch(mode){
        case RHIAddressMode::Wrap  : return SamplerAddressModeRepeat;
        case RHIAddressMode::Clamp : return SamplerAddressModeClampToEdge;
        case RHIAddressMode::Mirror: return SamplerAddressModeMirrorRepeat;
        case RHIAddressMode::Border: return SamplerAddressModeClampToBorderColor;
        default:
            std::unreachable();
        }
    }

    inline auto convertMinMagFilter(RHIFilter filter){
        using namespace MTL;

        switch(filter){
        case RHIFilter::Nearest: return SamplerMinMagFilterNearest;
        case RHIFilter::Linear:  return SamplerMinMagFilterLinear;
        default:
            std::unreachable();
        }
    }

    inline auto convertMipFilter(RHIFilter filter){
        using namespace MTL;

        switch(filter){
        case RHIFilter::Nearest: return SamplerMipFilterNearest;
        case RHIFilter::Linear:  return SamplerMipFilterLinear;
        default:
            std::unreachable();
        }
    }

    MetalSampler::MetalSampler(
        MTL::Device& device,
        const RHISamplerState& desc
    ){
        auto samplerDesc = MTL::SamplerDescriptor::alloc()->init();
        samplerDesc->setMinFilter(convertMinMagFilter(desc.minFilter));
        samplerDesc->setMagFilter(convertMinMagFilter(desc.magFilter));
        samplerDesc->setMipFilter(convertMipFilter(desc.magFilter));
        samplerDesc->setSAddressMode(convert(desc.addressU));
        samplerDesc->setTAddressMode(convert(desc.addressV));
        samplerDesc->setRAddressMode(convert(desc.addressW));
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
