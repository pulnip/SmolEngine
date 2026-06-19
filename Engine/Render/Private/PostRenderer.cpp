#include "Assert.hpp"
#include "PostRenderer.hpp"
#include "RHIBuffer.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"

namespace Smol
{
    PostRenderer::PostRenderer(RHIDevice& device)
        : rainStreak(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .topology = RHIPrimitiveTopology::TriangleStrip,
        #if defined(SMOL_DXRHI)
            .vertexShaderPath = "Engine/Shader/FullscreenQuad.vert.hlsl",
            .vertexShaderEntryPoint = "vs_main",
        #elif defined(SMOL_METALRHI)
            .vertexShaderPath = "Engine/Shader/FullscreenQuad.vert.metal",
            .vertexShaderEntryPoint = "vs_main",
        #endif
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
        #if defined(SMOL_DXRHI)
            .fragmentShaderPath = "Engine/Shader/RainStreak.pixel.hlsl",
            .fragmentShaderEntryPoint = "ps_main",
        #elif defined(SMOL_METALRHI)
            .fragmentShaderPath = "Engine/Shader/RainStreak.frag.metal",
            .fragmentShaderEntryPoint = "fs_main",
        #endif
            .blend = RHIBlendState{
                .alphaToCoverageEnable = false,
                .independentBlendEnable = false,
                .renderTargets = {
                    RHIRenderTargetBlendState{
                        .blendEnable = true,
                        .srcBlend = RHIBlend::One,
                        .dstBlend = RHIBlend::One,
                        .blendOp = RHIBlendOp::Add,
                        .srcBlendAlpha = RHIBlend::Zero,
                        .dstBlendAlpha = RHIBlend::One,
                        .blendOpAlpha = RHIBlendOp::Add
                    }
                }
            },
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
        }))
        , rainStreakParam(device.CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(RainCB),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite
        }, "rainStreakParam"))
    {
        SMOL_ASSERT(rainStreak != nullptr);
        SMOL_ASSERT(rainStreakParam != nullptr);

        auto& reflInfo = rainStreak->GetInfo();
        auto& fsInfo = reflInfo.fsInfo;
        auto& bufferInfo = fsInfo.bufferInfo;

        rainStreakParamSlot = bufferInfo.at("rainStreakParam").index;
    }

    PostRenderer::~PostRenderer() = default;

    void PostRenderer::Upload(const RainStreakParam& p0){
        rainStreakParam->Upload(&p0, sizeof(RainStreakParam));
    }

    void PostRenderer::Draw(RHICommandList& cmdList){
        cmdList.SetPipelineState(*rainStreak);

        cmdList.SetConstantBuffer(
            *rainStreakParam,
            rainStreakParamSlot,
            RHIShaderStage::FragmentShader
        );

        // FullscreenQuad with TriangleStrip
        cmdList.Draw(4);
    }
}
