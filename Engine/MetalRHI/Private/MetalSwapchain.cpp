#include <Metal/MTLCommandQueue.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include "Assert.hpp"
#include "MetalSwapchain.hpp"
#include "MetalUtil.hpp"

namespace Smol
{
    MetalSwapchain::MetalSwapchain(
        MTL::Device& device,
        const RHISwapchainCreateDesc& desc
    )
        : view(SDL_Metal_CreateView(static_cast<SDL_Window*>(desc.sdlWindow)))
        , metalLayer(static_cast<CA::MetalLayer*>(SDL_Metal_GetLayer(view)))
        , width(desc.bufferDesc.width)
        , height(desc.bufferDesc.height)
        , format(desc.bufferDesc.format)
    {
        SMOL_ASSERT(metalLayer != nullptr);

        metalLayer->setDevice(&device);
        metalLayer->setPixelFormat(convertPixelFormat(desc.bufferDesc.format));
        metalLayer->setFramebufferOnly(false);
        metalLayer->setDrawableSize(CGSizeMake(desc.bufferDesc.width, desc.bufferDesc.height));

        // NOTE. discard desc.debugName, desc.vsync
    }

    MetalSwapchain::~MetalSwapchain(){
        currentDrawable = nullptr;
    }

    bool MetalSwapchain::AcquireNextImage() noexcept{
        currentDrawable = metalLayer->nextDrawable();
        return currentDrawable != nullptr;
    }

    void MetalSwapchain::Resize(u32 newWidth, u32 newHeight){
        width = newWidth;
        height = newHeight;
        metalLayer->setDrawableSize(CGSizeMake(newWidth, newHeight));
        currentDrawable = nullptr;
    }

    void MetalSwapchain::Present(MTL::CommandBuffer& buffer) const{
        buffer.presentDrawable(currentDrawable);
    }

    MTL::Texture* MetalSwapchain::GetCurrentTexture() const noexcept{
        return currentDrawable ? currentDrawable->texture() : nullptr;
    }
}
