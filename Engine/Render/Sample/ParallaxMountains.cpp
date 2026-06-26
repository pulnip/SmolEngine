#include <print>
#include "Primitives.hpp"
#include "RHIBuffer.hpp"
#include "RHIDefinitions.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHIPipelineState.hpp"
#include "RHISwapchain.hpp"
#include "RuntimeConfig.hpp"
#include "SDLWindow.hpp"
#include "Timer.hpp"

using namespace Smol;

namespace{
    struct MountainLayer{
        f32 scrollSpeed;
        f32 baseHeight;
        f32 amplitude;
        f32 frequency;
        Color color;
    };

    constexpr auto NUM_LAYERS = 3;

    struct MountainParam{
        MountainLayer layers[NUM_LAYERS];
        // Sky Gradation
        Color skyTop;
        Color skyBottom;
        f32 elapsedTime;
        f32 pad[3];
    };
}

int main(void){
    try{
        auto device = CreateDevice();
        auto pipeline = device->CreatePipelineState(RHIGraphicsPipelineStateDesc{
            .preRasterizer = RHILegacyFrontendDesc{
                .topology = RHIPrimitiveTopology::TriangleStrip,
                .vertexShader = {
                #if defined(SMOL_DXRHI)
                    .path = "Engine/Shader/FullscreenQuad.vert.hlsl",
                    .entryPoint = "vs_main"
                #elif defined(SMOL_METALRHI)
                    .path = "Engine/Shader/FullscreenQuad.metal",
                    .entryPoint = "vs_main"
                #endif
                }
            },
            .rasterizer = RHIRasterizerState{
                .frontCounterClockwise = false
            },
            .fragmentShader = {
            #if defined(SMOL_DXRHI)
                .path = "Engine/Shader/ParallaxMountains.pixel.hlsl",
                .entryPoint = "ps_main"
            #elif defined(SMOL_METALRHI)
                .path = "Engine/Shader/ParallaxMountains.frag.metal",
                .entryPoint = "fs_main"
            #endif
            }
        });
        auto& reflInfo = pipeline->GetInfo();
        auto& fsInfo = reflInfo.fsInfo;
        auto& bufferInfo = fsInfo.bufferInfo;

        auto mountainParamSlot = bufferInfo.at("mountainParam").index;

        MountainParam mountainParam{
            .layers = {
                MountainLayer{
                    .scrollSpeed = 0.02f,
                    .baseHeight = 0.52f,
                    .amplitude = 0.12f,
                    .frequency = 3.0f,
                    .color = Color{0.698039f, 0.737255f, 0.788235f}
                },
                MountainLayer{
                    .scrollSpeed = 0.08f,
                    .baseHeight = 0.38f,
                    .amplitude = 0.15f,
                    .frequency = 5.0f,
                    .color = Color{0.474510f, 0.529412f, 0.607843f}
                },
                MountainLayer{
                    .scrollSpeed = 0.25f,
                    .baseHeight = 0.20f,
                    .amplitude = 0.18f,
                    .frequency = 7.0f,
                    .color = Color{0.247059f, 0.290196f, 0.352941f}
                }
            },
            .skyTop = Color{0.769f, 0.816f, 0.871f},
            .skyBottom = Color{0.925f, 0.894f, 0.839f},
            .elapsedTime = 0.0f
        };
        auto mountainParamBuf = device->CreateBuffer(RHIBufferCreateDesc{
            .size = sizeof(mountainParam),
            .usage = RHIBufferUsage::ConstantBuffer,
            .access = RHIMemoryAccess::CPUWrite,
            .initialData = &mountainParam
        }, "mountainParam");

        WindowConfig windowConfig{
            .title = "ParallaxMountains",
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

        Timer timer;
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

            timer.NewFrame();

            cmdList->Begin();

            mountainParam.elapsedTime = timer.GetElapsedTime();
            mountainParamBuf->Upload(mountainParam);

            std::array colorAttachments = {
                RHIColorAttachment{
                    .texture = &swapchain->GetCurrentTexture(),
                    .loadAction = RHILoadAction::Clear,
                    .storeAction = RHIStoreAction::Store,
                    .clearColor = Colors::Black
                }
            };
            cmdList->BeginRenderPass(RHIRenderPassDesc{
                .colorAttachments = colorAttachments
            });
            cmdList->SetViewport(RHIViewport{
                .x = 0, .y = 0,
                .width = static_cast<f32>(swapchain->GetWidth()),
                .height = static_cast<f32>(swapchain->GetHeight()),
                .minDepth = 0, .maxDepth = 1
            });

            cmdList->SetPipelineState(*pipeline);
            cmdList->SetConstantBuffer(
                *mountainParamBuf,
                mountainParamSlot,
                RHIShaderStage::FragmentShader
            );
            cmdList->Draw(4);

            cmdList->EndRenderPass();
            cmdList->Close();

            device->Submit(*cmdList, swapchain.get());
        }

        cmdList->WaitUntilCompleted();
    }
    catch(const std::exception& e){
        std::println("Exception: {}", e.what());

        return 1;
    }

    return 0;
}
