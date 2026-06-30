#include <array>
#include "ImmediateResourceLoader.hpp"
#include "Primitives.hpp"
#include "Resource.hpp"
#include "ResourceManager.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RuntimeConfig.hpp"
#include "SpriteRenderer.hpp"
#include "SDLWindow.hpp"

using namespace Smol;

CStr IMAGE_PATH = "Assets/Sprite/hollow_knight.png";

int main(void){
    auto device = CreateDevice();
    WindowConfig windowConfig{
        .title = "TextureSample",
        .width = 800, .height = 600,
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

    ImmediateResourceLoader resourceLoader(*device, "Content/");
    ResourceManager<SpriteResource> resourceManager(resourceLoader);
    SpriteRenderer renderer(*device, resourceManager);

    auto handle = resourceManager.Load(
        IMAGE_PATH,
        SpriteRequest{
            .path = IMAGE_PATH,
            .sheetSize = {16, 16},
            .animations = {
                {
                    "walk",
                    SpriteAnimation{
                        .startRow = 0,
                        .startCol = 0,
                        .frameCount = 8,
                        .frameDurationMs = 160
                    }
                }
            }
        }
    );
    auto proxy = renderer.BindRenderItem(handle);

    resourceManager.DrainCompletions();

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

        swapchain->AcquireNextImage();

        cmdList->Begin();
        std::array colorAttachments = {
            RHIColorAttachment{
                .texture = &swapchain->GetCurrentTexture(),
                .loadAction = RHILoadAction::Clear,
                .storeAction = RHIStoreAction::Store,
                .clearColor = clearColor
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

        renderer.Draw(*cmdList);

        cmdList->EndRenderPass();
        cmdList->Close();

        device->Submit(*cmdList);
        swapchain->Present();
    }

    cmdList->WaitUntilCompleted();

    return 0;
}
