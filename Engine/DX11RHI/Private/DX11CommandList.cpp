#include <stdexcept>
#include <utility>
#include <d3d11.h>
#include "Assert.hpp"
#include "DX11Buffer.hpp"
#include "DX11CommandList.hpp"
#include "DX11Definitions.hpp"
#include "DX11PipelineState.hpp"
#include "DX11Sampler.hpp"
#include "DX11Swapchain.hpp"
#include "DX11Texture.hpp"
#include "IntMath.hpp"
#include "RHIDefinitions.hpp"

namespace Smol
{
    DX11CommandList::DX11CommandList(DeviceContext& context)
        : context(context){}

    DX11CommandList::~DX11CommandList() = default;

    void DX11CommandList::Begin() noexcept{
        SMOL_ASSERT(!isRecording,
            "Did you call RHICommandList::close()?"
        );
        SMOL_ASSERT(!inRenderPass && !inComputePass);

        // NOTE. No-Op for DX11
        isRecording = true;
    }

    void DX11CommandList::Close() noexcept{
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(!inRenderPass && !inComputePass);

        // NOTE. No-Op for DX11
        isRecording = false;
    }

    void DX11CommandList::Reset() noexcept{
        if(isRecording){
            Flush();
            isRecording = false;
        }
    }

    void DX11CommandList::BeginRenderPass(
        std::span<RHITexture*> renderTargets,
        const RHIClearColor& clearColor,
        RHITexture* depthTarget,
        const RHIClearDepthStencil& clearDS,
        RHILoadAction loadAction,
        RHIStoreAction storeAction,
        CStr debugName
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(!inRenderPass,
            "Already in a render pass. Did you call RHICommandList::endRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass,
            "Already in a compute pass. Did you call RHICommandList::endComputePass()?"
        );
        SMOL_ASSERT(renderTargets.size() > 0);

        RTV* rtvs[RHI_MAX_RENDER_TARGETS];
        for(usize i=0; i<renderTargets.size(); ++i){
            auto tex = static_cast<DX11Texture*>(renderTargets[i]);
            rtvs[i] = tex->GetOrCreateRTV({.format = tex->GetFormat()});
        }

        DSV* dsv = nullptr;
        if(depthTarget != nullptr){
            auto tex = static_cast<DX11Texture*>(depthTarget);
            dsv = tex->GetOrCreateDSV({.format = tex->GetFormat()});
        }

        beginRenderPass(
            std::span<RTV*>(rtvs, renderTargets.size()), clearColor,
            dsv, clearDS,
            loadAction, storeAction,
            debugName
        );

        inRenderPass = true;
    }

    void DX11CommandList::BeginRenderPass(
        RHISwapchain& swapchain,
        const RHIClearColor& clearColor,
        RHITexture* depthTarget,
        const RHIClearDepthStencil& clearDS,
        RHILoadAction loadAction,
        RHIStoreAction storeAction,
        CStr debugName
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(!inRenderPass,
            "Already in a render pass. Did you call RHICommandList::endRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass,
            "Already in a compute pass. Did you call RHICommandList::endComputePass()?"
        );
        ID3D11RenderTargetView* rtvs[1] = {
            static_cast<DX11Swapchain&>(swapchain).GetCurrentRTV()
        };

        DSV* dsv = nullptr;
        if(depthTarget != nullptr){
            auto tex = static_cast<DX11Texture*>(depthTarget);
            dsv = tex->GetOrCreateDSV({.format = tex->GetFormat()});
        }

        beginRenderPass(
            rtvs, clearColor,
            dsv, clearDS,
            loadAction, storeAction,
            debugName
        );

        inRenderPass = true;
    }

    void DX11CommandList::EndRenderPass(){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        // NOTE. No-Op for DX11
    #if defined(_DEBUG) || !defined(NDEBUG)
        // clean-up srv binding for suppress data hazard warning.
        ID3D11ShaderResourceView* nullSRVs[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};

        if(maxBindedVSSRV > 0)
            context.VSSetShaderResources(0, maxBindedVSSRV, nullSRVs);
        if(maxBindedPSSRV > 0)
            context.PSSetShaderResources(0, maxBindedPSSRV, nullSRVs);
        if(maxBindedCSSRV > 0)
            context.CSSetShaderResources(0, maxBindedCSSRV, nullSRVs);

        maxBindedVSSRV = 0;
        maxBindedPSSRV = 0;
        maxBindedCSSRV = 0;

        // Unbind render targets to avoid warnings about resources being bound while being used.
        context.OMSetRenderTargets(0, nullptr, nullptr);
    #endif

        inRenderPass = false;
    }

    void DX11CommandList::SetPipelineState(RHIGraphicsPipelineState& pso){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        auto& dxPSO = static_cast<DX11GraphicsPipelineState&>(pso);
        dxPSO.Bind(context);
    }

    void DX11CommandList::SetPipelineState(RHIComputePipelineState& pso){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inComputePass,
            "Not in a compute pass. Did you call RHICommandList::beginComputePass()?"
        );
        SMOL_ASSERT(!inRenderPass);

        auto& dxPSO = static_cast<DX11ComputePipelineState&>(pso);
        dxPSO.Bind(context);

        currentComputePSO = &dxPSO;
    }

    void DX11CommandList::SetVertexBuffer(
        const RHIBuffer& buffer,
        u32 slot,
        u32 stride,
        u32 offset
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        auto buf = static_cast<const DX11Buffer&>(buffer).Get();
        context.IASetVertexBuffers(
            slot,
            1,
            &buf,
            &stride,
            &offset
        );
    }

    void DX11CommandList::SetIndexBuffer(
        const RHIBuffer& buffer,
        RHIIndexFormat format,
        u32 offset
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        auto buf = static_cast<const DX11Buffer&>(buffer).Get();
        context.IASetIndexBuffer(
            buf,
            format == RHIIndexFormat::UInt16 ?
                DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT,
            offset
        );
    }

    void DX11CommandList::SetConstantBuffer(
        const RHIBuffer& buffer,
        u32 slot,
        RHIShaderStage stage,
        u32 offset
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass != inComputePass,
            "Not in a pass. Did you call RHICommandList::beginPass()?"
        );

        using enum RHIShaderStage;

        auto buf = static_cast<const DX11Buffer&>(buffer).Get();

        switch(stage){
        case VertexShader:
            context.VSSetConstantBuffers(
                slot,
                1,
                &buf
            );
            break;
        case FragmentShader:
            context.PSSetConstantBuffers(
                slot,
                1,
                &buf
            );
            break;
        case ComputeShader:
            context.CSSetConstantBuffers(
                slot,
                1,
                &buf
            );
            break;
        default:
            std::unreachable();
        }
    }

    void DX11CommandList::SetTexture(
        RHITexture& texture,
        u32 slot,
        RHIBindingAccess access,
        RHIShaderStage stage
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass != inComputePass,
            "Not in a pass. Did you call RHICommandList::beginPass()?"
        );

        using enum RHIBindingAccess;
        using enum RHIShaderStage;
        auto& dxTex = static_cast<DX11Texture&>(texture);

        switch(access){
        case ReadOnly: {
            const auto view = dxTex.GetOrCreateSRV({
                .format = texture.GetFormat()
            });
            switch(stage){
            case VertexShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedVSSRV = std::max(maxBindedVSSRV, slot+1);
            #endif
                context.VSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            case FragmentShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedPSSRV = std::max(maxBindedPSSRV, slot+1);
            #endif
                context.PSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            case ComputeShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedCSSRV = std::max(maxBindedCSSRV, slot+1);
            #endif
                context.CSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            default:
                std::unreachable();
            }
        } break;
        case ReadWrite: {
            SMOL_ASSERT(stage == ComputeShader);
            const auto view = dxTex.GetOrCreateUAV({
                .format = texture.GetFormat()
            });
            switch(stage){
            case ComputeShader:
                context.CSSetUnorderedAccessViews(
                    slot,
                    1,
                    &view,
                    nullptr
                );
                break;
            default:
                std::unreachable();
            }
        } break;
        default:
            std::unreachable();
        }
    }

    void DX11CommandList::SetBuffer(
        RHIBuffer& buffer,
        u32 slot,
        RHIBindingAccess access,
        RHIShaderStage stage
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass != inComputePass,
            "Not in a pass. Did you call RHICommandList::beginPass()?"
        );

        using enum RHIBindingAccess;
        using enum RHIShaderStage;
        auto& dxBuf = static_cast<DX11Buffer&>(buffer);

        switch(access){
        case ReadOnly: {
            const auto view = dxBuf.GetOrCreateSRV(RHIBufferViewDesc{
                .size = buffer.GetSize()
            });
            switch(stage){
            case VertexShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedVSSRV = std::max(maxBindedVSSRV, slot+1);
            #endif
                context.VSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            case FragmentShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedPSSRV = std::max(maxBindedPSSRV, slot+1);
            #endif
                context.PSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            case ComputeShader:
            #if defined(_DEBUG) || !defined(NDEBUG)
                maxBindedCSSRV = std::max(maxBindedCSSRV, slot+1);
            #endif
                context.CSSetShaderResources(
                    slot,
                    1,
                    &view
                );
                break;
            default:
                std::unreachable();
            }
        } break;
        case ReadWrite: {
            // cannot bind to VS, GS, HS, DS, TS
            // TODO. bind to PS is available at OMSetRenderTargetsAndUnorderedAccessViews
            SMOL_ASSERT(stage == ComputeShader);
            const auto view = dxBuf.GetOrCreateUAV(RHIBufferViewDesc{
                .size = buffer.GetSize()
            });
            switch(stage){
            case ComputeShader:
                context.CSSetUnorderedAccessViews(
                    slot,
                    1,
                    &view,
                    nullptr
                );
                break;
            default:
                std::unreachable();
            }
        } break;
        default:
            std::unreachable();
        }
    }

    void DX11CommandList::SetBytes(
        const void* bytes,
        u32 slot,
        usize size,
        RHIShaderStage stage
    ){
        using enum RHIShaderStage;

        throw std::runtime_error("Unimplemented");

        switch(stage){
        case VertexShader:
            [[fallthrough]];
        case FragmentShader:
            break;
        case ComputeShader:
            break;
        default:
            std::unreachable();
        }
    }

    void DX11CommandList::SetSampler(
        const RHISampler& sampler,
        u32 slot,
        RHIShaderStage stage
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass != inComputePass,
            "Not in a pass. Did you call RHICommandList::beginPass()?"
        );

        using enum RHIShaderStage;
        auto s = static_cast<const DX11Sampler&>(sampler).Get();

        switch(stage){
        case VertexShader:
            context.VSSetSamplers(slot, 1, &s);
            break;
        case FragmentShader:
            context.PSSetSamplers(slot, 1, &s);
            break;
        case ComputeShader:
            context.CSSetSamplers(slot, 1, &s);
            break;
        default:
            std::unreachable();
        }
    }

    void DX11CommandList::SetViewport(const RHIViewport& viewport){
        D3D11_VIEWPORT vp{
            .TopLeftX = viewport.x,
            .TopLeftY = viewport.y,
            .Width = viewport.width,
            .Height = viewport.height,
            .MinDepth = viewport.minDepth,
            .MaxDepth = viewport.maxDepth
        };
        context.RSSetViewports(
            1,
            &vp
        );
    }

    void DX11CommandList::SetScissorRect(const RHIScissorRect& scissor){
        D3D11_RECT rect{
            .left = scissor.left,
            .top = scissor.top,
            .right = scissor.right,
            .bottom = scissor.bottom
        };
        context.RSSetScissorRects(
            1,
            &rect
        );
    }

    void DX11CommandList::Draw(
        u32 vertexCount,
        u32 instanceCount,
        u32 startVertex,
        u32 startInstance
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        if(instanceCount > 1)
            context.DrawInstanced(
                vertexCount,
                instanceCount,
                startVertex,
                startInstance
            );
        else
            context.Draw(
                vertexCount,
                startVertex
            );
    }

    void DX11CommandList::DrawIndexed(
        u32 indexCount,
        u32 instanceCount,
        u32 startIndex,
        i32 baseVertex,
        u32 startInstance
    ){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inRenderPass,
            "Not in a render pass. Did you call RHICommandList::beginRenderPass()?"
        );
        SMOL_ASSERT(!inComputePass);

        if(instanceCount > 1)
            context.DrawIndexedInstanced(
                indexCount,
                instanceCount,
                startIndex,
                baseVertex,
                startInstance
            );
        else
            context.DrawIndexed(
                indexCount,
                startIndex,
                baseVertex
            );
    }

    void DX11CommandList::BeginCompute() noexcept{
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(!inComputePass,
            "Already in a compute pass. Did you call RHICommandList::endComputePass()?"
        );
        SMOL_ASSERT(!inRenderPass,
            "Already in a render pass. Did you call RHICommandList::endRenderPass()?"
        );

        inComputePass = true;
        currentComputePSO = nullptr;
    }

    void DX11CommandList::EndCompute() noexcept{
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inComputePass,
            "Not in a compute pass. Did you call RHICommandList::beginComputePass()?"
        );
        SMOL_ASSERT(!inRenderPass);

        inComputePass = false;
    }

    void DX11CommandList::Dispatch(RHISize3D gridSize){
        SMOL_ASSERT(isRecording,
            "Did you call RHICommandList::begin()?"
        );
        SMOL_ASSERT(inComputePass,
            "Not in a compute pass. Did you call RHICommandList::beginComputePass()?"
        );
        SMOL_ASSERT(currentComputePSO != nullptr,
            "Did you call RHICommandList::setPipelineState(ComputePSO)?"
        );
        auto threadGroupSize = currentComputePSO->getThreadGroupSize();

        context.Dispatch(
            ceilDiv(gridSize.x, threadGroupSize.x),
            ceilDiv(gridSize.y, threadGroupSize.y),
            ceilDiv(gridSize.z, threadGroupSize.z)
        );
    }

    void DX11CommandList::Copy(
        RHIBuffer& src,
        RHIBuffer& dst,
        usize srcOffset,
        usize dstOffset,
        usize size
    ){
        D3D11_BOX box{
            .left = static_cast<UINT>(srcOffset),
            .top = 0,
            .front = 0,
            .right = static_cast<UINT>(srcOffset + size),
            .bottom = 1,
            .back = 1
        };

        context.CopySubresourceRegion(
            static_cast<DX11Buffer&>(dst).Get(), 0,
            dstOffset, 0, 0,
            static_cast<DX11Buffer&>(src).Get(), 0,
            &box
        );
    }

    void DX11CommandList::Copy(
        RHITexture& src,
        RHITexture& dst
    ){
        context.CopyResource(
            static_cast<DX11Texture&>(dst).Get(),
            static_cast<DX11Texture&>(src).Get()
        );
    }

    void DX11CommandList::Copy(
        RHITexture& src,
        RHISwapchain& dst
    ){
        context.CopyResource(
            static_cast<DX11Swapchain&>(dst).GetCurrentTexture(),
            static_cast<DX11Texture&>(src).Get()
        );
    }

    void DX11CommandList::Copy(
        RHIBuffer& src,
        RHITexture& dst,
        u32 mipLevel,
        u32 arraySlice
    ){
        // TODO
    }

    void DX11CommandList::beginRenderPass(
        std::span<ID3D11RenderTargetView*> rtvs,
        const RHIClearColor& clearColor,
        ID3D11DepthStencilView* dsv,
        const RHIClearDepthStencil& clearDS,
        RHILoadAction loadAction,
        RHIStoreAction storeAction,
        CStr debugName
    ){
        context.OMSetRenderTargets(
            rtvs.size(),
            rtvs.data(),
            dsv
        );

        if(loadAction == RHILoadAction::Clear){
            for(usize i=0; i<rtvs.size(); ++i)
                context.ClearRenderTargetView(rtvs[i], clearColor.v);

            if(dsv != nullptr)
                context.ClearDepthStencilView(dsv,
                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                    clearDS.depth, clearDS.stencil
                );
        }
    }
}
