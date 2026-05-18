#pragma once

#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHISwapchain.hpp"
#include "DX11Definitions.hpp"

namespace Smol
{
    class DX11Swapchain final: public RHISwapchain{
    private:
        SwapchainRAII swapchain = nullptr;
        bool vsync, allowTearing;
        // Back buffer and RTV for rendering
        TextureRAII backBuffer = nullptr;
        RTVRAII rtv = nullptr;
        // cache for creating rtv
        Device& device;

        u32 width = 0;
        u32 height = 0;
        RHIPixelFormat format = RHIPixelFormat::Unknown;

    public:
        DX11Swapchain(
            Device& device,
            Factory& factory,
            const RHISwapchainCreateDesc& desc
        );

        ~DX11Swapchain();

        bool AcquireNextImage() noexcept RHI_OVERRIDE{
            return true;
        }

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

        void Present() const RHI_OVERRIDE;

        void* GetCurrentNativeTexture() const noexcept RHI_OVERRIDE{
            return GetCurrentTexture();
        }

        Texture* GetCurrentTexture() const noexcept;
        RTV* GetCurrentRTV() const noexcept;

    private:
        void createBackBufferResource();
    };
}
