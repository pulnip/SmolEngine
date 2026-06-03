#include "Assert.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"
#include "RHISampler.hpp"
#include "SpriteAnimation.hpp"
#include "SpriteRenderer.hpp"

namespace Smol
{
    SpriteRenderer::SpriteRenderer(RHIDevice& device)
        : pipeline(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .topology = RHIPrimitiveTopology::TriangleStrip,
        #if defined(SMOL_DXRHI)
            .vertexShaderPath = "Engine/Shader/SpriteQuad.vert.hlsl",
            .vertexShaderEntryPoint = "vs_main",
        #elif defined(SMOL_METALRHI)
        #endif
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
        #if defined(SMOL_DXRHI)
            .fragmentShaderPath = "Engine/Shader/Sprite.pixel.hlsl",
            .fragmentShaderEntryPoint = "ps_main",
        #elif defined(SMOL_METALRHI)
        #endif
            // Alpha blending
            .blend = RHIBlendState{
                .independentBlendEnable = false,
                .renderTargets = {
                    RHIRenderTargetBlendState{
                        .blendEnable = true,
                        .srcBlend = RHIBlend::SrcAlpha,
                        .dstBlend = RHIBlend::InvSrcAlpha
                    }
                }
            }
        }))
        , sampler(device.CreateSampler(LINEAR_WRAP_SAMPLER))
    {
        SMOL_ASSERT(pipeline != nullptr);
        SMOL_ASSERT(sampler != nullptr);

        auto& reflInfo = pipeline->GetInfo();

        auto& vsInfo = reflInfo.vsInfo;
        auto& spriteConstantsBind = vsInfo.bufferInfo.at(vs.SpriteConstantsSlot);
        vs.spriteConstants = spriteConstantsBind.index;

        auto& fsInfo = reflInfo.fsInfo;
        auto& texBind = fsInfo.textureInfo.at(fs.texSlot);
        auto& sampBind = fsInfo.samplerInfo.at(fs.sampSlot);

        fs.tex = texBind.index;
        SMOL_ASSERT(texBind.access == RHIBindingAccess::ReadOnly);
        fs.samp = sampBind.index;
    }

    SpriteRenderer::~SpriteRenderer() = default;

    struct SpriteConstants{
        Vec2 uvScale;
        Vec2 offset;
    };
    inline SpriteConstants pack(const SpriteRenderItem& item){
        return SpriteConstants{
            .uvScale = item.uvScale,
            .offset = item.offset
        };
    }

    void SpriteRenderer::Draw(
        RHICommandList& cmdList,
        std::span<const SpriteRenderItem> items
    ){
        using enum RHIShaderStage;

        cmdList.SetPipelineState(*pipeline);

        cmdList.SetSampler(*sampler,
            fs.samp,
            RHIShaderStage::FragmentShader
        );

        for(auto& item: items){
            auto c = pack(item);
            cmdList.SetBytes(
                &c,
                vs.spriteConstants,
                sizeof(c),
                RHIShaderStage::VertexShader
            );

            cmdList.SetTexture(
                item.texture,
                fs.tex,
                RHIBindingAccess::ReadOnly,
                RHIShaderStage::FragmentShader
            );
            cmdList.Draw(4);
        }

    }
}
