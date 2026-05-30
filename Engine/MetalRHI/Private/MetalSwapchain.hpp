#pragma once

#include <Metal/MTLCommandQueue.hpp>
#include <Metal/MTLDevice.hpp>
#include <Metal/MTLTexture.hpp>
#include <QuartzCore/CAMetalDrawable.hpp>
#include <SDL3/SDL_metal.h>
#include "Primitives.hpp"
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHISwapchain.hpp"

namespace Smol
{
    class MetalSwapchain final: public RHISwapchain{
    private:
        SDL_MetalView view;
        // Cache MetalLayer
        CA::MetalLayer* metalLayer = nullptr;
        CA::MetalDrawable* currentDrawable = nullptr;

        u32 width = 0;
        u32 height = 0;
        RHIPixelFormat format = RHIPixelFormat::Unknown;

    public:
        MetalSwapchain(
            MTL::Device& device,
            const RHISwapchainCreateDesc& desc
        );

        ~MetalSwapchain();

        bool AcquireNextImage() noexcept RHI_OVERRIDE;

        void Resize(u32 newWidth, u32 newHeight) RHI_OVERRIDE;

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE{
            return format;
        }
        u32 GetWidth() const noexcept RHI_OVERRIDE{
            return width;
        }
        u32 GetHeight() const noexcept RHI_OVERRIDE{
            return height;
        }

        void Present(MTL::CommandBuffer&) const;

        void* GetCurrentNativeTexture() const noexcept RHI_OVERRIDE{
            return GetCurrentTexture();
        }

        MTL::Texture* GetCurrentTexture() const noexcept;

        CA::MetalDrawable* GetCurrentDrawable() const noexcept{
            return currentDrawable;
        }
    };
}
