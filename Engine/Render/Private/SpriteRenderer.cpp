#include "Assert.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"
#include "SpriteProxy.hpp"
#include "SpriteRenderer.hpp"

namespace{
    struct SpriteConstants{
        Smol::Vec2 uvScale;
        Smol::Vec2 offset;
    };

    auto pack(const Smol::SpriteRenderItem& item){
        return SpriteConstants{
            .uvScale = item.uvScale,
            .offset = item.offset
        };
    }
}

namespace Smol
{
    SpriteRenderer::SpriteRenderer(RHIDevice& device)
        : pipeline(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .topology = RHIPrimitiveTopology::TriangleStrip,
        #if defined(SMOL_DXRHI)
            .vertexShaderPath = "Engine/Shader/SpriteQuad.vert.hlsl",
            .vertexShaderEntryPoint = "vs_main",
        #elif defined(SMOL_METALRHI)
            .vertexShaderPath = "Engine/Shader/SpriteQuad.metal",
            .vertexShaderEntryPoint = "vs_main",
        #endif
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
        #if defined(SMOL_DXRHI)
            .fragmentShaderPath = "Engine/Shader/Sprite.pixel.hlsl",
            .fragmentShaderEntryPoint = "ps_main",
        #elif defined(SMOL_METALRHI)
            .fragmentShaderPath = "Engine/Shader/SpriteQuad.metal",
            .fragmentShaderEntryPoint = "fs_main",
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

    SpriteProxy SpriteRenderer::BindRenderItem(RHITexture& texture){
        auto handle = renderItems.emplace(SpriteRenderItem{
            // TODO. change later
            .uvScale = {1.0f/16, 1.0f/16},
            .offset = {.x = 0, .y = 0},
            .texture = texture,
        });

        return SpriteProxy(handle, *this);
    }

    SpriteRenderItem& SpriteRenderer::GetRenderItem(Handle handle){
        return renderItems.get(handle);
    }

    void SpriteRenderer::UnbindRenderItem(Handle handle){
        renderItems.remove(handle);
    }

    void SpriteRenderer::Draw(RHICommandList& cmdList){
        using enum RHIShaderStage;

        cmdList.SetPipelineState(*pipeline);

        cmdList.SetSampler(*sampler,
            fs.samp,
            RHIShaderStage::FragmentShader
        );

        for(auto& item: renderItems){
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
