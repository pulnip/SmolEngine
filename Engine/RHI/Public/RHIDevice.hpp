#pragma once

#include "CoreFWD.hpp"
#include "Semantics.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    using NativeDeviceHandle = void*;

    class RHIDevice{
    private:
        class Impl;
        RAII<Impl> impl;

    public:
        RHIDevice();
        virtual ~RHIDevice();
        SMOL_DECLARE_PINNED(RHIDevice)

        void BeginFrame();
        void EndFrame();

        virtual RHIFrameScopeRAII CreateFrameScope() = 0;

        virtual RHIBufferRAII CreateBuffer(
            const RHIBufferCreateDesc&,
            StrView name = {}
        ) = 0;
        virtual RHITextureRAII CreateTexture(
            const RHITextureCreateDesc&,
            StrView name = {}
        ) = 0;
        virtual RHISamplerRAII CreateSampler(
            const RHISamplerState&
        ) = 0;

        virtual RHIGraphicsPipelineStateRAII CreatePipelineState(
            const RHIGraphicsPipelineStateDesc&,
            StrView name = {}
        ) = 0;
        virtual RHIComputePipelineStateRAII CreatePipelineState(
            const RHIComputePipelineStateDesc&,
            StrView name = {}
        ) = 0;

        virtual RHISwapchainRAII CreateSwapchain(
            const RHISwapchainCreateDesc&
        ) = 0;

        virtual RHICommandListRAII CreateCommandList() = 0;

        virtual RHIFenceRAII CreateFence(u64 initialValue = 0) = 0;

        virtual void SignalFence(RHIFence&, u64 value) = 0;

        virtual void Submit(RHICommandList&) = 0;

        virtual RHICapabilities GetCapabilities() const noexcept = 0;

        // return native type for UI
        // in Metal, MTL::Device
        virtual NativeDeviceHandle Get() noexcept = 0;
        // DeviceContext for DX11, CommandQueue for DX12
        // not used at Metal
        virtual void* GetContextOrQueue() noexcept{ return nullptr; };
    };

    // each platform should implement this function
    RHIDeviceRAII CreateDevice();
}
