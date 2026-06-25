#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHITexture.hpp"
#include "MetalUtil.hpp"

namespace Smol
{
    class MetalTexture final: public RHITexture{
    private:
        MTL::Texture* texture;
        RHIResourceState currentState = RHIResourceState::Common;

    public:
        MetalTexture(
            MTL::Device& device,
            const RHITextureCreateDesc& desc,
            StrView name = {}
        );
        MetalTexture(
            CA::MetalDrawable*
        );
        ~MetalTexture();

        void Upload(
            const void* data,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) RHI_OVERRIDE;

        MTL::Texture* Get() const{ return texture; }

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE{
            return convert(texture->pixelFormat());
        }
        usize GetWidth() const noexcept RHI_OVERRIDE{
            return texture->width();
        }
        usize GetHeight() const noexcept RHI_OVERRIDE{
            return texture->height();
        }

        virtual void* GetNative() const noexcept RHI_OVERRIDE{
            return texture;
        }

        RHIResourceState GetState() const noexcept RHI_OVERRIDE{
            return currentState;
        }

        void SetState(RHIResourceState state) noexcept RHI_OVERRIDE{
            currentState = state;
        }
    };
}
