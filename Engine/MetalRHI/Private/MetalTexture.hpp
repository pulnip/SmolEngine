#pragma once

#include <Metal/Metal.hpp>
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHITexture.hpp"

namespace Smol
{
    class MetalTexture final: public RHITexture{
    private:
        MTL::Texture* texture;
        usize width, height;
        RHIPixelFormat format = RHIPixelFormat::Unknown;
        RHIResourceState currentState = RHIResourceState::Common;

    public:
        MetalTexture(
            MTL::Device& device,
            const RHITextureCreateDesc& desc,
            StrView name = {}
        );
        ~MetalTexture();

        void Upload(
            const void* data,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) RHI_OVERRIDE;

        MTL::Texture* Get() const{ return texture; }

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE{
            return format;
        }
        usize GetWidth() const noexcept RHI_OVERRIDE{
            return width;
        }
        usize GetHeight() const noexcept RHI_OVERRIDE{
            return height;
        }

        RHIResourceState GetState() const noexcept RHI_OVERRIDE{
            return currentState;
        }

        void SetState(RHIResourceState state) noexcept RHI_OVERRIDE{
            currentState = state;
        }
    };
}
