#pragma once

#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    // Command list for recording GPU commands
    class RHICommandList{
    public:
        SMOL_DECLARE_INTERFACE(RHICommandList)

        // Command list lifecycle
        virtual void Begin() = 0;
        virtual void Close() = 0;

        // Render pass control
        virtual void BeginRenderPass(const RHIRenderPassDesc&) = 0;
        virtual void EndRenderPass() = 0;

        // Pipeline state
        virtual void SetPipelineState(RHIGraphicsPipelineState&) = 0;
        virtual void SetPipelineState(RHIComputePipelineState&) = 0;

        // Vertex buffer
        // stride = sizeof(Vertex)
        virtual void SetVertex(
            RHIBuffer&,
            u32 slot,
            u32 stride,
            u32 offset = 0
        ) = 0;

        // Index buffer
        virtual void SetIndex(
            RHIBuffer&,
            RHIIndexFormat format = RHIIndexFormat::UInt32,
            u32 offset = 0
        ) = 0;

        // ConstantBuffer
        virtual void SetVertexConstant(
            RHIBuffer&,
            u32 slot
        ) = 0;
        virtual void SetFragmentConstant(
            RHIBuffer&,
            u32 slot
        ) = 0;

        // for per-draw data, size should be <= 256B
        virtual void SetVertexBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) = 0;
        virtual void SetFragmentBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) = 0;

        // type-safe helper
        template<typename T>
            requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
        void SetVertexBytes(
            const T& data,
            u32 slot
        ){
            SetVertexBytes(&data, sizeof(T), slot);
        }
        template<typename T>
            requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
        void SetFragmentBytes(
            const T& data,
            u32 slot
        ){
            SetFragmentBytes(&data, sizeof(T), slot);
        }

        // Shader resource
        virtual void SetVertexReadable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetVertexReadable(
            RHIBuffer&,
            u32 slot
        ) = 0;
        virtual void SetFragmentReadable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetFragmentReadable(
            RHIBuffer&,
            u32 slot
        ) = 0;

        // Unordered Access
        virtual void SetVertexWritable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetVertexWritable(
            RHIBuffer&,
            u32 slot
        ) = 0;
        virtual void SetFragmentWritable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetFragmentWritable(
            RHIBuffer&,
            u32 slot
        ) = 0;

        // sampler
        virtual void SetVertexSampler(
            RHISampler&,
            u32 slot
        ) = 0;
        virtual void SetFragmentSampler(
            RHISampler&,
            u32 slot
        ) = 0;

        // Viewport and scissor
        virtual void SetViewport(const RHIViewport&) = 0;
        virtual void SetScissorRect(const RHIScissorRect&) = 0;

        // Draw commands
        virtual void Draw(
            u32 vertexCount,
            u32 instanceCount = 1,
            u32 startVertex = 0,
            u32 startInstance = 0
        ) = 0;

        virtual void DrawIndexed(
            u32 indexCount,
            u32 instanceCount = 1,
            u32 startIndex = 0,
            i32 baseVertex = 0,
            u32 startInstance = 0
        ) = 0;

        virtual void BeginCompute() = 0;
        virtual void EndCompute() = 0;

        virtual void SetComputeConstant(
            RHIBuffer&,
            u32 slot
        ) = 0;

        virtual void SetComputeBytes(
            const void* bytes,
            usize size,
            u32 slot
        ) = 0;

        // type-safe helper
        template<typename T>
            requires (!std::is_pointer_v<T> && std::is_trivially_copyable_v<T>)
        void SetComputeBytes(
            const T& data,
            u32 slot
        ){
            SetVertexBytes(&data, sizeof(T), slot);
        }

        virtual void SetComputeReadable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetComputeReadable(
            RHIBuffer&,
            u32 slot
        ) = 0;
        virtual void SetComputeWritable(
            RHITexture&,
            u32 slot
        ) = 0;
        virtual void SetComputeWritable(
            RHIBuffer&,
            u32 slot
        ) = 0;

        virtual void SetComputeSampler(
            RHISampler&,
            u32 slot
        ) = 0;

        // Compute dispatch
        virtual void Dispatch(
            Size3D gridSize
        ) = 0;

        virtual void BeginBlit() = 0;
        virtual void EndBlit() = 0;

        // Copy operations
        virtual void Copy(
            RHIBuffer& src,
            RHIBuffer& dst,
            usize srcOffset,
            usize dstOffset,
            usize size
        ) = 0;

        virtual void Copy(
            RHITexture& src,
            RHITexture& dst
        ) = 0;

        // helper for RHISwapchain(backBuffer)
        void Copy(
            RHITexture& src,
            RHISwapchain& dst
        );

        virtual void Copy(
            RHIBuffer& src,
            RHITexture& dst,
            u32 mipLevel = 0,
            u32 arraySlice = 0
        ) = 0;

        virtual void WaitUntilCompleted() = 0;

        // Debug markers (for GPU profiling)
        virtual void BeginEvent(CStr name) = 0;
        virtual void EndEvent() = 0;
        virtual void SetMarker(CStr name) = 0;

        // for UI,
        //   DeviceContext for D3D11,
        //   CommandBuffer for Metal
        virtual void* GetNative() noexcept = 0;
    };
}
