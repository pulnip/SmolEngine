#pragma once

#include <d3d11_1.h>
#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHICommandList.hpp"
#include "DX11Definitions.hpp"
#include "DX11PipelineState.hpp"
#include "DX11Buffer.hpp"

namespace Smol
{
    class DX11CommandList: public RHICommandList{
    private:
        DeviceContextRAII context;
        // simulate command recording
        bool isRecording = false;
        bool inRenderPass = false;
        bool inComputePass = false;
        bool inBlitPass = false;
        DX11ComputePipelineState* currentComputePSO = nullptr;
    #if defined(_DEBUG) || !defined(NDEBUG)
        COMRAII<ID3DUserDefinedAnnotation> annotation;

        u32 maxBindedVSSRV = 0;
        u32 maxBindedPSSRV = 0;
        u32 maxBindedCSSRV = 0;
    #endif

        DX11Buffer inlineBuffer;

    public:
        // DX11CommandList with immediate context
        DX11CommandList(Device& device);
        // DX11CommandList with deferred context
        DX11CommandList(Device& device, DeviceContext& immediateContext);
        ~DX11CommandList();

        void Begin() noexcept RHI_OVERRIDE;
        void Close() noexcept RHI_OVERRIDE;

        void BeginRenderPass(const RHIRenderPassDesc&) RHI_OVERRIDE;
        void EndRenderPass() RHI_OVERRIDE;

        void SetPipelineState(RHIGraphicsPipelineState& pso) RHI_OVERRIDE;
        void SetPipelineState(RHIComputePipelineState& pso) RHI_OVERRIDE;

        void SetVertex(
            RHIBuffer&,
            u32 slot,
            u32 stride,
            u32 offset
        ) RHI_OVERRIDE;

        void SetIndex(
            RHIBuffer&,
            RHIIndexFormat,
            u32 offset
        ) RHI_OVERRIDE;

        void SetVertexConstant(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentConstant(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetVertexBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) RHI_OVERRIDE;

        void SetVertexReadable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetVertexReadable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentReadable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentReadable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetVertexWritable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetVertexWritable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentWritable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentWritable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetVertexSampler(
            RHISampler&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetFragmentSampler(
            RHISampler&,
            u32 slot
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

        void SetComputeConstant(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetComputeBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) RHI_OVERRIDE;

        void SetComputeReadable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetComputeReadable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetComputeWritable(
            RHITexture&,
            u32 slot
        ) RHI_OVERRIDE;
        void SetComputeWritable(
            RHIBuffer&,
            u32 slot
        ) RHI_OVERRIDE;

        void SetComputeSampler(
            RHISampler&,
            u32 slot
        ) RHI_OVERRIDE;

        void Dispatch(Size3D gridSize) RHI_OVERRIDE;

        void BeginBlit() noexcept RHI_OVERRIDE;
        void EndBlit() noexcept RHI_OVERRIDE;

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
            RHIBuffer& src,
            RHITexture& dst,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) RHI_OVERRIDE;

        void WaitUntilCompleted() noexcept RHI_OVERRIDE{
            // NOTE. No-Op for DX11
        }

        void BeginEvent(CStr name) RHI_OVERRIDE;
        void EndEvent() RHI_OVERRIDE;
        void SetMarker(CStr name) RHI_OVERRIDE;

        void* GetNative() noexcept RHI_OVERRIDE;

        COMRAII<ID3D11CommandList> Finish();

        DeviceContext& Get(){ return *context.Get(); }
    };
}
