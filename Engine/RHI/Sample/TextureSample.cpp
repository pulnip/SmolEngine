#include "ImageLoader.hpp"
#include "Primitives.hpp"
#include "RHIDefinitions.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"
#include "RHISampler.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RuntimeConfig.hpp"
#include "Window.hpp"

using namespace Smol;

CStr IMAGE_PATH = "Content/Assets/Sprite/hollow_knight.png";

int main(void){
    auto device = CreateDevice();
    auto pipeline = device->CreatePipelineState(RHIGraphicsPipelineStateDesc{
        .topology = RHIPrimitiveTopology::TriangleStrip,
    #if defined(SMOL_DXRHI)
        .vertexShaderPath = "Engine/Shader/FullscreenQuad.vert.hlsl",
        .vertexShaderEntryPoint = "vs_main",
    #elif defined(SMOL_METALRHI)
    #endif
        .rasterizer = RHIRasterizerState{
            .frontCounterClockwise = false
        },
    #if defined(SMOL_DXRHI)
        .fragmentShaderPath = "Engine/Shader/Sprite.pixel.hlsl",
        .fragmentShaderEntryPoint = "ps_main"
    #elif defined(SMOL_METALRHI)
    #endif
    });

    WindowConfig windowConfig{
        .title = "TextureSample",
        .width = 800, .height = 800,
        .fullscreen = false,
        .resizable = false,
    };
    Window window(windowConfig);
    auto fWidth = static_cast<f32>(window.GetWidth());
    auto fHeight = static_cast<f32>(window.GetHeight());

    RHITextureCreateDesc backBufferDesc{
        .width = windowConfig.width, .height = windowConfig.height,
        .format = RHIPixelFormat::RGBA8_UNORM
    };
    auto swapchain = device->CreateSwapchain(RHISwapchainCreateDesc{
        .sdlWindow = window.GetWindow(),
        .bufferDesc = backBufferDesc
    });

    // load Texture from Image
    auto image = loadImage(IMAGE_PATH);
    auto texture = device->CreateTexture(RHITextureCreateDesc{
        .width = image.GetWidth(), .height = image.GetHeight(),
        .format = RHIPixelFormat::RGBA8_UNORM,
        .usage = RHITextureUsage::AllowShaderRead,
        .initialData = image.GetBufferPointer()
    });
    auto sampler = device->CreateSampler(LINEAR_WRAP_SAMPLER);

    auto cmdList = device->CreateCommandList();

    while(true){
        if(!window.ProcessEvents()) [[unlikely]]
            break;

        cmdList->Begin();
        cmdList->BeginRenderPass(*swapchain);

        cmdList->SetPipelineState(*pipeline);
        cmdList->SetViewport(RHIViewport{
            .x = 0, .y = 0,
            .width = fWidth, .height = fHeight,
            .minDepth = 0, .maxDepth = 1
        });

        cmdList->SetTexture(*texture, 0,
            RHIBindingAccess::ReadOnly,
            RHIShaderStage::FragmentShader
        );
        cmdList->SetSampler(*sampler, 0,
            RHIShaderStage::FragmentShader
        );

        cmdList->Draw(4);

        cmdList->EndRenderPass();
        cmdList->Close();

        device->Submit(*cmdList, swapchain.get());
    }

    cmdList->WaitUntilCompleted();

    return 0;
}
