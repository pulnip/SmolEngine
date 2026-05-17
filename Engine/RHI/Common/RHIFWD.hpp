#pragma once

#include "CoreFWD.hpp"

namespace Smol
{
    class RHIDevice;
    class RHIBuffer;
    class RHITexture;
    class RHISampler;
    class RHICommandList;
    class RHIFence;
    class RHIFrameScope;
    class RHIGraphicsPipelineState;
    class RHIComputePipelineState;
    class RHISwapchain;

    class FramePacer;

    using RHIDeviceRAII = RAII<RHIDevice>;
    using RHIBufferRAII = RAII<RHIBuffer>;
    using RHITextureRAII = RAII<RHITexture>;
    using RHISamplerRAII = RAII<RHISampler>;
    using RHICommandListRAII = RAII<RHICommandList>;
    using RHIFenceRAII = RAII<RHIFence>;
    using RHIFrameScopeRAII = RAII<RHIFrameScope>;
    using RHIGraphicsPipelineStateRAII = RAII<RHIGraphicsPipelineState>;
    using RHIComputePipelineStateRAII = RAII<RHIComputePipelineState>;
    using RHISwapchainRAII = RAII<RHISwapchain>;
    using FramePacerRAII = RAII<FramePacer>;
}
