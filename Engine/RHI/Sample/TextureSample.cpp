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
#include "SDLWindow.hpp"

using namespace Smol;

CStr IMAGE_PATH = "Content/Assets/Sprite/hollow_knight.png";

int main(void){
    auto device = CreateDevice();
    auto pipeline = device->CreatePipelineState(RHIGraphicsPipelineStateDesc{
        .preRasterizer = RHILegacyFrontendDesc{
            .topology = RHIPrimitiveTopology::TriangleStrip,
            .vertexShader = {
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
        .fragmentShader = {
        #if defined(SMOL_DXRHI)
            .path = "Engine/Shader/Sprite.pixel.hlsl",
            .entryPoint = "ps_main"
        #elif defined(SMOL_METALRHI)
            .path = "Engine/Shader/SpriteQuad.metal",
            .entryPoint = "fs_main"
        #endif
        },
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
    });

    WindowConfig windowConfig{
        .title = "TextureSample",
        .width = 800, .height = 800,
        .fullscreen = false,
        .resizable = false,
    };
    SDLWindow window(windowConfig);

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

    Color clearColor = Colors::Grey;
    auto cmdList = device->CreateCommandList();

    while(true){
        bool keepRunning = true;

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_EVENT_QUIT: [[unlikely]]
                keepRunning = false;
                break;
            }

            if(!keepRunning) [[unlikely]]
                break;

            if(SDL_EVENT_WINDOW_FIRST <= event.type && event.type <= SDL_EVENT_WINDOW_LAST){
                if(event.type == SDL_EVENT_WINDOW_RESIZED){
                    int w = event.window.data1;
                    int h = event.window.data2;
                    swapchain->Resize(w, h);
                }
                window.OnPlatformEvent(event.window);
            }
        }

        if(!keepRunning) [[unlikely]]
            break;

        cmdList->Begin();
        cmdList->BeginRenderPass(*swapchain,
            clearColor,
            nullptr,
            {},
            RHILoadAction::Clear
        );

        cmdList->SetPipelineState(*pipeline);
        cmdList->SetViewport(RHIViewport{
            .x = 0, .y = 0,
            .width = static_cast<f32>(swapchain->GetWidth()),
            .height = static_cast<f32>(swapchain->GetHeight()),
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
