#include "RHICommandList.hpp"
#include "RHISwapchain.hpp"

namespace Smol
{
    void RHICommandList::Copy(
        RHITexture& src,
        RHISwapchain& dst
    ){
        auto& backBuffer = dst.GetCurrentTexture();
        Copy(
            src,
            backBuffer
        );
    }
}
