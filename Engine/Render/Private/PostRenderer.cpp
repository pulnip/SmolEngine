#include "Assert.hpp"
#include "CameraConfig.hpp"
#include "PostRenderer.hpp"
#include "RHIBuffer.hpp"
#include "RHICommandList.hpp"
#include "RHIDefinitions.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"
#include "RHISampler.hpp"
#include "RHITexture.hpp"

namespace Smol
{
    PostRenderer::PostRenderer(RHIDevice& device)
        : rainStreakPipeline(device.CreatePipelineState(RHIGraphicsPipelineStateDesc{
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
            .renderTargetFormats = {
                RHIPixelFormat::RGBA8_UNORM
            },
            .renderTargetCount = 1
        }))
        , linearClamp(device.CreateSampler(LINEAR_CLAMP_SAMPLER))
        , rainCB(device.CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(RainCB),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite
        }, "RainCB"))
    {
        SMOL_ASSERT(rainStreakPipeline != nullptr);
        SMOL_ASSERT(linearClamp != nullptr);
        SMOL_ASSERT(rainCB != nullptr);

        auto& reflInfo = rainStreakPipeline->GetInfo();
        auto& fsInfo = reflInfo.fsInfo;
        auto& textureInfo = fsInfo.textureInfo;
        auto& samplerInfo = fsInfo.samplerInfo;
        auto& bufferInfo = fsInfo.bufferInfo;

        fs.tex = textureInfo.at(fs.texSlot).index;
        fs.linearClamp = samplerInfo.at(fs.linearClampSlot).index;
        fs.rainCB = bufferInfo.at(fs.rainCBSlot).index;
    }

    PostRenderer::~PostRenderer() = default;

    void PostRenderer::Upload(const RainCB& param){
        rainCB->Upload(&param, sizeof(RainCB));
    }

    void PostRenderer::Draw(RHICommandList& cmdList, RHITexture& scene){
        cmdList.SetPipelineState(*rainStreakPipeline);

        cmdList.SetConstantBuffer(
            *rainCB,
            fs.rainCB,
            RHIShaderStage::FragmentShader
        );
        cmdList.SetTexture(
            scene,
            fs.tex,
            RHIBindingAccess::ReadOnly,
            RHIShaderStage::FragmentShader
        );
        cmdList.SetSampler(
            *linearClamp,
            fs.linearClamp,
            RHIShaderStage::FragmentShader
        );

        // FullscreenQuad with TriangleStrip
        cmdList.Draw(4);
    }
}
