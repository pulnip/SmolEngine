#include <d3d11.h>
#include <SDL3/SDL_video.h>
#include <stdexcept>
#include "DX11Definitions.hpp"
#include "DX11Util.hpp"
#include "DX11Swapchain.hpp"
#include "DX11Texture.hpp"

namespace Smol
{
    DX11Swapchain::DX11Swapchain(
        Device& device,
        Factory& factory,
        const RHISwapchainCreateDesc& desc
    )
        : device(device)
        , vsync(desc.vsync)
        , allowTearing(desc.allowTearing)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
            .Width = desc.bufferDesc.width,
            .Height = desc.bufferDesc.height,
            .Format = convert(desc.bufferDesc.format),
            .Stereo = FALSE,
            // No MSAA for swapchain
            .SampleDesc = {1, 0},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = desc.bufferCount,
            .Scaling = DXGI_SCALING_STRETCH,
            .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
            .AlphaMode = DXGI_ALPHA_MODE_IGNORE,
            .Flags = desc.allowTearing ?
                DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : UINT(0)
        };

        auto sdlWindow = static_cast<SDL_Window*>(desc.sdlWindow);
        auto hWnd = SDL_GetPointerProperty(
            SDL_GetWindowProperties(sdlWindow),
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            nullptr
        );
        factory.CreateSwapChainForHwnd(
            &device,
            static_cast<HWND>(hWnd),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapchain
        );

        backBuffer = std::make_unique<DX11Texture>(
            device,
            *swapchain.Get()
        );

    #if defined(_DEBUG) || !defined(NDEBUG)
        if(!desc.debugName.empty()){
            swapchain->SetPrivateData(
                WKPDID_D3DDebugObjectName,
                static_cast<UINT>(desc.debugName.length()),
                desc.debugName.c_str()
            );
        }
    #endif
    }

    DX11Swapchain::~DX11Swapchain() = default;

    void DX11Swapchain::Resize(u32 newWidth, u32 newHeight){
        if(newWidth == 0 || newHeight == 0)
            return;

        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);

        backBuffer.reset();
        if(FAILED(swapchain->ResizeBuffers(
            0, newWidth, newHeight,
            DXGI_FORMAT_UNKNOWN, desc.Flags
        ))){
            throw std::runtime_error("Failed to resize Swapchain buffer");
        }

        // recreate back buffer resource
        backBuffer = std::make_unique<DX11Texture>(
            device,
            *swapchain.Get()
        );
    }

    RHIPixelFormat DX11Swapchain::GetFormat() const noexcept{
        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);

        return convert(desc.Format);
    }

    u32 DX11Swapchain::GetWidth() const noexcept{
        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);

        return desc.Width;
    }

    u32 DX11Swapchain::GetHeight() const noexcept{
        DXGI_SWAP_CHAIN_DESC1 desc;
        swapchain->GetDesc1(&desc);

        return desc.Height;
    }

    void DX11Swapchain::Present(){
        UINT syncInterval = vsync ? 1 : 0;
        UINT flags = (!vsync && allowTearing) ?
            DXGI_PRESENT_ALLOW_TEARING : 0;

        swapchain->Present(
            syncInterval,
            flags
        );
    }
}
