#include "Assert.hpp"
#include "CameraConfig.hpp"
#include "ResourceManager.hpp"
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
        Smol::Mat4 mvp;
        Smol::Vec2 uvScale;
        Smol::Vec2 offset;
    };

    auto pack(const Smol::SpriteRenderItem& item, const Smol::Size2D& size){
        using namespace Smol;

        auto model = modelMat(
            item.transform.position,
            item.transform.rotation,
            item.transform.scale * item.spriteScale
        );

        return SpriteConstants{
            .mvp = mvp(model, detail::view, detail::ogProj),
            .uvScale = {1.0f/size.x, 1.0f/size.y},
            .offset = item.offset
        };
    }
}

namespace Smol
{
    SpriteRenderer::SpriteRenderer(
        RHIDevice& device,
        ResourceManager<SpriteResource>& spriteManager
    )
        : spriteManager(spriteManager)
        , pipeline(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .preRasterizer = RHILegacyFrontendDesc{
                .topology = RHIPrimitiveTopology::TriangleStrip,
                .vertexShader = RHIShaderDesc{
                #if defined(SMOL_DXRHI)
                    .path = "Engine/Shader/SpriteQuad.vert.hlsl",
                    .entryPoint = "vs_main"
                #elif defined(SMOL_METALRHI)
                    .path = "Engine/Shader/SpriteQuad.metal",
                    .entryPoint = "vs_main"
                #endif
                }
            },
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
            .fragmentShader = RHIShaderDesc{
            #if defined(SMOL_DXRHI)
                .path = "Engine/Shader/Sprite.pixel.hlsl",
                .entryPoint = "ps_main"
            #elif defined(SMOL_METALRHI)
                .path = "Engine/Shader/SpriteQuad.metal",
                .entryPoint = "fs_main"
            #endif
            },
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
            },
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
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

    SpriteProxy SpriteRenderer::BindRenderItem(
        ResourceHandle resourceHandle
    ){
        auto handle = renderItems.Emplace(SpriteRenderItem{
            .handle = resourceHandle,
        });

        return SpriteProxy(handle, *this);
    }

    SpriteRenderItem& SpriteRenderer::GetRenderItem(Handle handle){
        return renderItems.GetRef(handle);
    }

    void SpriteRenderer::UnbindRenderItem(Handle handle){
        renderItems.Remove(handle);
    }

    void SpriteRenderer::Draw(RHICommandList& cmdList){
        using enum RHIShaderStage;

        cmdList.SetPipelineState(*pipeline);

        cmdList.SetSampler(*sampler,
            fs.samp,
            RHIShaderStage::FragmentShader
        );

        for(auto& item: renderItems){
            auto& resource = spriteManager.GetRef(item.handle);

            auto c = pack(item, resource.sheetSize);
            cmdList.SetBytes(
                c,
                vs.spriteConstants,
                RHIShaderStage::VertexShader
            );


            cmdList.SetTexture(
                *resource.texture,
                fs.tex,
                RHIBindingAccess::ReadOnly,
                RHIShaderStage::FragmentShader
            );
            cmdList.Draw(4);
        }
    }
}
