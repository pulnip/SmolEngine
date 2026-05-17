#pragma once

#include "RHIAPI.hpp"
#include "RHIDefinitions.hpp"
#include "RHIPipelineState.hpp"
#include "DX11Definitions.hpp"

namespace Smol
{
    class DX11GraphicsPipelineState: public RHIGraphicsPipelineState{
    private:
        InputLayoutRAII inputLayout = nullptr;
        D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
        VertexShaderRAII vertexShader = nullptr;
        RasterizerStateRAII rasterizerState = nullptr;
        PixelShaderRAII pixelShader = nullptr;
        DepthStencilStateRAII depthStencilState = nullptr;
        BlendStateRAII blendState = nullptr;

        RHIGraphicsBindingInfo bindingInfo;

    #if defined(_DEBUG) || !defined(NDEBUG)
        const Str debugName;
    #endif

    public:
        DX11GraphicsPipelineState(
            Device& device,
            const RHIGraphicsPipelineStateDesc& desc,
            StrView name = {}
        );

        ~DX11GraphicsPipelineState();

        const RHIGraphicsBindingInfo& GetInfo() const RHI_OVERRIDE{
            return bindingInfo;
        }

        void Bind(DeviceContext& ctx) const;
    };

    class DX11ComputePipelineState: public RHIComputePipelineState{
    private:
        ComputeShaderRAII computeShader = nullptr;
        RHIComputeBindingInfo bindingInfo;
        RHISize3D threadGroupSize = {256, 1, 1};

    #if defined(_DEBUG) || !defined(NDEBUG)
        const std::string debugName;
    #endif

    public:
        DX11ComputePipelineState(
            Device& device,
            const RHIComputePipelineStateDesc& desc,
            StrView name = {}
        );

        ~DX11ComputePipelineState();

        const RHIComputeBindingInfo& GetInfo() const RHI_OVERRIDE{
            return bindingInfo;
        }

        void Bind(DeviceContext& ctx) const;

        RHISize3D getThreadGroupSize() const{
            return threadGroupSize;
        }
    };
}
