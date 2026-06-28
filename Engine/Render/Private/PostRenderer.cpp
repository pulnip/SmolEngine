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
            .preRasterizer = RHILegacyFrontendDesc{
                .topology = RHIPrimitiveTopology::TriangleStrip,
                .vertexShader = RHIShaderDesc{
                #if defined(SMOL_DXRHI)
                    .path = "Engine/Shader/FullscreenQuad.vert.hlsl",
                    .entryPoint = "vs_main"
                #elif defined(SMOL_METALRHI)
                    .path = "Engine/Shader/FullscreenQuad.vert.metal",
                    .entryPoint = "vs_main"
                #endif
                }
            },
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
            .fragmentShader = RHIShaderDesc{
            #if defined(SMOL_DXRHI)
                .path = "Engine/Shader/RainStreak.pixel.hlsl",
                .entryPoint = "ps_main"
            #elif defined(SMOL_METALRHI)
                .path = "Engine/Shader/RainStreak.frag.metal",
                .entryPoint = "fs_main"
            #endif
            },
            .blend = RHIBlendState{
                .alphaToCoverageEnable = false,
                .independentBlendEnable = false,
                .renderTargets = {
                    RHIRenderTargetBlendState{
                        .blendEnable = true,
                        .srcBlend = RHIBlend::One,
                        .dstBlend = RHIBlend::One,
                        .blendOp = RHIBlendOp::Add,
                        // anyway, Not use Alpha channel
                        .srcBlendAlpha = RHIBlend::Zero,
                        .dstBlendAlpha = RHIBlend::One,
                        .blendOpAlpha = RHIBlendOp::Add,
                        .writeMask = RHIColorWriteMask::EnableColor
                    }
                }
            },
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
        }))
        , rainStreakParam(device.CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(RainStreakParam),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite
        }, "rainStreakParam")),
        rainDroplet(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .preRasterizer = RHILegacyFrontendDesc{
                .topology = RHIPrimitiveTopology::TriangleStrip,
                .vertexShader = RHIShaderDesc{
                #if defined(SMOL_DXRHI)
                    .path = "Engine/Shader/FullscreenQuad.vert.hlsl",
                    .entryPoint = "vs_main"
                #elif defined(SMOL_METALRHI)
                    .path = "Engine/Shader/FullscreenQuad.vert.metal",
                    .entryPoint = "vs_main"
                #endif
                }
            },
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
            .fragmentShader = RHIShaderDesc{
            #if defined(SMOL_DXRHI)
                .path = "Engine/Shader/RainDroplet.pixel.hlsl",
                .entryPoint = "ps_main"
            #elif defined(SMOL_METALRHI)
                .path = "Engine/Shader/RainDroplet.frag.metal",
                .entryPoint = "fs_main"
            #endif
            },
            .blend = RHIBlendState{
                .alphaToCoverageEnable = false,
                .independentBlendEnable = false,
                .renderTargets = {
                    RHIRenderTargetBlendState{
                        .blendEnable = true,
                        .srcBlend = RHIBlend::One,
                        .dstBlend = RHIBlend::SrcAlpha,
                        .blendOp = RHIBlendOp::Add,
                        // anyway, Not use Alpha channel
                        .srcBlendAlpha = RHIBlend::Zero,
                        .dstBlendAlpha = RHIBlend::One,
                        .blendOpAlpha = RHIBlendOp::Add,
                        .writeMask = RHIColorWriteMask::EnableColor
                    }
                }
            },
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
        }))
        , rainDropletParam(device.CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(RainDropletParam),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite
        }, "rainDropletParam"))
    {
        SMOL_ASSERT(rainStreak != nullptr);
        SMOL_ASSERT(rainStreakParam != nullptr);
        SMOL_ASSERT(rainDroplet != nullptr);
        SMOL_ASSERT(rainDropletParam != nullptr);

        {
            auto& reflInfo = rainStreak->GetInfo();
            auto& fsInfo = reflInfo.fsInfo;
            auto& bufferInfo = fsInfo.bufferInfo;

            rainStreakParamSlot = bufferInfo.at("rainStreakParam").index;
        }

        {
            auto& reflInfo = rainDroplet->GetInfo();
            auto& fsInfo = reflInfo.fsInfo;
            auto& bufferInfo = fsInfo.bufferInfo;

            rainStreakParamSlot = bufferInfo.at("rainDropletParam").index;
        }
    }

    PostRenderer::~PostRenderer() = default;

    void PostRenderer::Upload(const RainStreakParam& p0){
        rainStreakParam->Upload(p0);

        RainDropletParam p1{};
        rainDropletParam->Upload(p1);
    }

    void PostRenderer::Draw(RHICommandList& cmdList){
        // Rain Streak
        cmdList.SetPipelineState(*rainStreak);

        cmdList.SetConstantBuffer(
            *rainStreakParam,
            rainStreakParamSlot,
            RHIShaderStage::FragmentShader
        );

        // FullscreenQuad with TriangleStrip
        cmdList.Draw(4);

        // Rain Droplet
        cmdList.SetPipelineState(*rainDroplet);
        cmdList.SetConstantBuffer(
            *rainDropletParam,
            rainDropletParamSlot,
            RHIShaderStage::FragmentShader
        );
        cmdList.Draw(4);
    }
}
