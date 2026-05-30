#include <d3d11.h>
#include <SDL3/SDL_video.h>
#include "DX11Definitions.hpp"
#include "DX11Util.hpp"
#include "DX11Swapchain.hpp"

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
        , width(desc.bufferDesc.width)
        , height(desc.bufferDesc.height)
        , format(desc.bufferDesc.format)
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
            .Width = desc.bufferDesc.width,
            .Height = desc.bufferDesc.height,
            .Format = convertPixelFormat(desc.bufferDesc.format),
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
        createBackBufferResource();
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

        swapchain->ResizeBuffers(
            0, newWidth, newHeight,
            DXGI_FORMAT_UNKNOWN, desc.Flags
        );

        createBackBufferResource();
    }

    void DX11Swapchain::Present() const{
        UINT syncInterval = vsync ? 1 : 0;
        UINT flags = (!vsync && allowTearing) ?
            DXGI_PRESENT_ALLOW_TEARING : 0;

        swapchain->Present(syncInterval, flags);
    }

    Texture* DX11Swapchain::GetCurrentTexture() const noexcept{
        return backBuffer.Get();
    }

    RTV* DX11Swapchain::GetCurrentRTV() const noexcept{
        return rtv.Get();
    }

    void DX11Swapchain::createBackBufferResource(){
        swapchain->GetBuffer(0, __uuidof(Texture),
            reinterpret_cast<void**>(backBuffer.GetAddressOf())
        );
        device.CreateRenderTargetView(backBuffer.Get(), nullptr, &rtv);
    }
}
