#pragma once

#include <Metal/MTLDevice.hpp>
#include <Metal/MTLSampler.hpp>
#include "RHIDefinitions.hpp"
#include "RHISampler.hpp"

namespace Smol
{

    class MetalSampler final: public RHISampler{
    private:
        MTL::SamplerState* sampler = nullptr;

    public:
        MetalSampler(
            MTL::Device& device,
            const RHISamplerState& desc
        );

        ~MetalSampler();

        MTL::SamplerState* Get() const{ return sampler; }
    };
}
