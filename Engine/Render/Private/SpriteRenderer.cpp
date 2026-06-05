#include "Assert.hpp"
#include "LinearAlgebra.hpp"
#include "Primitives.hpp"
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

    // TODO. CameraComponent
    constexpr Smol::Vec3 camPos(0.0f, 0.0f, -1.0f);
    const auto camRot = Smol::unitQuat();
    const auto view = Smol::viewMat(camPos, camRot);

    constexpr auto aspect = 800.0f/600.0f;
    constexpr auto fovY = Smol::toRadian(50.0f);
    constexpr auto nearZ = 0.1f;
    constexpr auto farZ = 100.0f;
    // perspective projection template
    const auto ppProj = Smol::perspective(fovY, aspect, nearZ, farZ);
    // for perspective <-> orthographic convert
    constexpr auto focusDist = 30.0f;
    const auto halfH = focusDist * std::tanf(0.5f * fovY);
    const auto ogProj = Smol::orthographic(
        2.0f*halfH*aspect,
        2.0f*halfH,
        nearZ,
        farZ
    );

    auto pack(const Smol::SpriteRenderItem& item){
        using namespace Smol;

        auto model = modelMat(item.transform);

        return SpriteConstants{
            .mvp = mvp(model, view, ogProj),
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

    SpriteProxy SpriteRenderer::BindRenderItem(RHITexture& texture){
        auto handle = renderItems.emplace(SpriteRenderItem{
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
