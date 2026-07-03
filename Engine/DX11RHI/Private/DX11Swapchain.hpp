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

        Device& device;
        RHITextureRAII backBuffer;

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

        RHIPixelFormat GetFormat() const noexcept RHI_OVERRIDE;
        u32 GetWidth() const noexcept RHI_OVERRIDE;
        u32 GetHeight() const noexcept RHI_OVERRIDE;

        RHITexture& GetCurrentTexture() RHI_OVERRIDE{
            return *backBuffer;
        }
        const RHITexture& GetCurrentTexture() const RHI_OVERRIDE{
            return *backBuffer;
        }

        void Present(RHICommandList&) RHI_OVERRIDE;
    };
}
