#pragma once

#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHICommandList.hpp"
#include "DX11Definitions.hpp"
#include "DX11PipelineState.hpp"

namespace Smol
{
    class DX11CommandList: public RHICommandList{
    private:
        // NOTE. Immediate context.
        DeviceContext& context;
        // simulate command recording
        bool isRecording = false;
        bool inRenderPass = false, inComputePass = false;
        DX11ComputePipelineState* currentComputePSO = nullptr;
    #if defined(_DEBUG) || !defined(NDEBUG)
        u32 maxBindedVSSRV = 0;
        u32 maxBindedPSSRV = 0;
        u32 maxBindedCSSRV = 0;
    #endif

    public:
        DX11CommandList(DeviceContext& context);
        ~DX11CommandList();

        void Begin() noexcept RHI_OVERRIDE;
        void Flush() noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }
        void Close() noexcept RHI_OVERRIDE;
        void Reset() noexcept RHI_OVERRIDE;

        void BeginRenderPass(
            std::span<RHITexture*> renderTargets,
            const RHIClearColor& clearColor,
            RHITexture* depthTarget,
            const RHIClearDepthStencil& clearDS,
            RHILoadAction loadAction,
            RHIStoreAction storeAction,
            CStr debugName
        ) RHI_OVERRIDE;

        void BeginRenderPass(
            RHISwapchain& swapchain,
            const RHIClearColor& clearColor,
            RHITexture* depthTarget,
            const RHIClearDepthStencil& clearDS,
            RHILoadAction loadAction,
            RHIStoreAction storeAction,
            CStr debugName
        ) RHI_OVERRIDE;

        void EndRenderPass() RHI_OVERRIDE;

        void SetPipelineState(RHIGraphicsPipelineState& pso) RHI_OVERRIDE;
        void SetPipelineState(RHIComputePipelineState& pso) RHI_OVERRIDE;

        void SetVertexBuffer(
            const RHIBuffer& buffer,
            u32 slot,
            u32 stride,
            u32 offset
        ) RHI_OVERRIDE;

        void SetIndexBuffer(
            const RHIBuffer& buffer,
            RHIIndexFormat format,
            u32 offset
        ) RHI_OVERRIDE;

        void SetConstantBuffer(
            const RHIBuffer& buffer,
            u32 slot,
            RHIShaderStage stage,
            u32 offset
        ) RHI_OVERRIDE;

        void SetTexture(
            RHITexture& texture,
            u32 slot,
            RHIBindingAccess access,
            RHIShaderStage stage
        ) RHI_OVERRIDE;

        void SetBuffer(
            RHIBuffer& buffer,
            u32 slot,
            RHIBindingAccess access,
            RHIShaderStage stage = RHIShaderStage::ComputeShader
        ) RHI_OVERRIDE;

        void SetBytes(
            const void* bytes,
            u32 slot,
            usize size,
            RHIShaderStage stage = RHIShaderStage::ComputeShader
        ) RHI_OVERRIDE;

        void SetSampler(
            const RHISampler& sampler,
            u32 slot,
            RHIShaderStage stage
        ) RHI_OVERRIDE;

        void SetViewport(const RHIViewport& viewport) RHI_OVERRIDE;
        void SetScissorRect(const RHIScissorRect& scissor) RHI_OVERRIDE;

        void Draw(
            u32 vertexCount,
            u32 instanceCount = 1,
            u32 startVertex = 0,
            u32 startInstance = 0
        ) RHI_OVERRIDE;

        void DrawIndexed(
            u32 indexCount,
            u32 instanceCount = 1,
            u32 startIndex = 0,
            i32 baseVertex = 0,
            u32 startInstance = 0
        ) RHI_OVERRIDE;

        void BeginCompute() noexcept RHI_OVERRIDE;
        void EndCompute() noexcept RHI_OVERRIDE;

        void Dispatch(RHISize3D gridSize) RHI_OVERRIDE;

        void TransitionBarrier(
            RHITexture&,
            RHIResourceState after
        ) noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }

        void TransitionBarrier(
            RHIBuffer&,
            RHIResourceState after
        ) noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }

        void Copy(
            RHIBuffer& src,
            RHIBuffer& dst,
            usize srcOffset,
            usize dstOffset,
            usize size
        ) RHI_OVERRIDE;

        void Copy(
            RHITexture& src,
            RHITexture& dst
        ) RHI_OVERRIDE;

        void Copy(
            RHITexture& src,
            RHISwapchain& dst
        ) RHI_OVERRIDE;

        void Copy(
            RHIBuffer& src,
            RHITexture& dst,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) RHI_OVERRIDE;

        void WaitUntilCompleted() noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }

        void BeginEvent(CStr name) noexcept RHI_OVERRIDE{
            // TODO
        }
        void EndEvent() noexcept RHI_OVERRIDE{
            // TODO
        }
        void SetMarker(CStr name) noexcept RHI_OVERRIDE{
            // TODO
        }

    private:
        void beginRenderPass(
            std::span<ID3D11RenderTargetView*> rtvs,
            const RHIClearColor& clearColor,
            ID3D11DepthStencilView* dsv,
            const RHIClearDepthStencil& clearDS,
            RHILoadAction loadAction,
            RHIStoreAction storeAction,
            CStr debugName
        );
    };
}
