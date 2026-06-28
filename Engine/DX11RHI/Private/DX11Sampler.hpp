#pragma once

#include "RHIDefinitions.hpp"
#include "RHISampler.hpp"
#include "DX11Definitions.hpp"

namespace Smol
{
    class DX11Sampler: public RHISampler{
    private:
        SamplerRAII sampler = nullptr;

    public:
        DX11Sampler(
            Device& device,
            const RHISamplerState& desc
        );

        ~DX11Sampler();

        Sampler* Get() noexcept{ return sampler.Get(); }
        const Sampler* Get() const noexcept{ return sampler.Get(); }
    };
}
