#pragma once

#include "RHIDefinitions.hpp"
#include "RHIFWD.hpp"

namespace Smol
{
    struct MaterialDesc{
        RHIShaderDesc vertexShader{
            .entryPoint = "vs_main"
        };
        RHIRasterizerState rasterizer = {};
        RHIShaderDesc fragmentShader{
            .entryPoint = "fs_main"
        };
        std::optional<RHIDepthStencilState> depthStencil = std::nullopt;
        std::optional<RHIBlendState> blend = std::nullopt;
    };
}

template<>
struct std::hash<Smol::MaterialDesc>{
    std::size_t operator()(const Smol::MaterialDesc& desc) const noexcept{
        using namespace Smol;

        std::size_t h = hashAll(
            desc.vertexShader,
            desc.rasterizer,
            desc.fragmentShader
        );

        if(desc.depthStencil.has_value()){
            h = hashAll(h, *desc.depthStencil);
        }
        if(desc.blend.has_value()){
            h = hashAll(h, *desc.blend);
        }

        return h;
    }
};

namespace Smol
{
    struct MaterialPipelineState{
        // TODO. use compiled RHIShader
        RHIShaderDesc vertexShader{
            .entryPoint = "vs_main"
        };
        RHIRasterizerState rasterizer = {};
        RHIShaderDesc fragmentShader{
            .entryPoint = "fs_main"
        };
        std::optional<RHIDepthStencilState> depthStencil = std::nullopt;
        std::optional<RHIBlendState> blend = std::nullopt;
    };

    class Material{
    public:
        Material(const MaterialDesc& desc);

        const MaterialPipelineState& PipelineState() const{ return ps; }

    private:
        MaterialPipelineState ps;
        RHIBufferRAII paramBuffer;
        u32 paramSlot;
    };
}
