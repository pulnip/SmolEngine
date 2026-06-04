#include "ActorRegistry.hpp"
#include "Assert.hpp"
#include "InputConfig.hpp"
#include "InputManager.hpp"
#include "InputModifier.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"
#include "RHITexture.hpp"
#include "RuntimeConfig.hpp"
#include "SpawnContext.hpp"
#include "SpriteRenderer.hpp"
#include "Timer.hpp"
#include "SDLWindow.hpp"
#include "SDLInputProvider.hpp"
#include "World.hpp"

using namespace Smol;

int main(int, char*[]){
    WindowConfig windowConfig{
        .title = "ActorUpdateSample",
        .width = 800,
        .height = 600
    };
    SDLWindow window(windowConfig);
    auto fWidth = static_cast<f32>(window.GetWidth());
    auto fHeight = static_cast<f32>(window.GetHeight());

    SDLInputProvider inputProvider;

    // Initialize Input Feature
    InputConfig inputConfig{
        .mappings = {
            ActionInfo{
                .name = "Move",
                .mappings = {
                    KeyBinding{
                        .keyCode = KeyCode::W,
                        .modifiers = {
                            SwizzleModifier(YXZ)
                        }
                    },
                    KeyBinding{
                        .keyCode = KeyCode::A,
                        .modifiers = {
                            NegateModifier(
                                true,
                                false,
                                false
                            )
                        }
                    },
                    KeyBinding{
                        .keyCode = KeyCode::S,
                        .modifiers = {
                            SwizzleModifier(YXZ),
                            NegateModifier(
                                false,
                                false,
                                true
                            ),
                        }
                    },
                    KeyBinding{
                        .keyCode = KeyCode::D
                    }
                },
                .count = 0
            }
        }
    };
    InputManager inputManager(
        inputConfig,
        &inputProvider
    );

    // Initialize Render Feature
    auto device = CreateDevice();
    RHITextureCreateDesc backBufferDesc{
        .width = windowConfig.width, .height = windowConfig.height,
        .format = RHIPixelFormat::RGBA8_UNORM
    };
    auto swapchain = device->CreateSwapchain(
        RHISwapchainCreateDesc{
            .sdlWindow = window.GetWindow(),
            .bufferDesc = backBufferDesc
        }
    );
    SpriteRenderer renderer(*device);

    // Initialize Update Feature
    // TODO. use World class
    SpawnContext context{
        .inputManager = inputManager,
        .device = device.get(),
        .spriteRenderer = &renderer
    };
    auto controller = CreateActor("CharacterController", context);
    SMOL_ASSERT(controller != nullptr);
    auto player = CreateActor("SimplePlayer", context);
    SMOL_ASSERT(player != nullptr);

    auto cmdList = device->CreateCommandList();
    RHIClearColor clearColor{.v = {
        0.5f, 0.5f, 0.5f, 1.0f
    }};
    Timer timer;

    while(true){
        bool keepRunning = true;
        inputProvider.NewFrame();

        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type){
            case SDL_EVENT_QUIT: [[unlikely]]
                keepRunning = false;
                break;
            // Keyboard Event
            case SDL_EVENT_KEY_DOWN:
                [[fallthrough]];
            case SDL_EVENT_KEY_UP:
                inputProvider.OnPlatformEvent(event.key);
                break;
            }

            if(!keepRunning) [[unlikely]]
                break;

            // Window Event
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

        inputManager.NewFrame();

        // Update World
        timer.NewFrame();

        auto dt = timer.GetDeltaTime();
        controller->OnUpdate(dt);
        player->OnUpdate(dt);

        controller->SyncRenderState();
        player->SyncRenderState();

        // Render
        cmdList->Begin();
        cmdList->BeginRenderPass(*swapchain,
            clearColor,
            nullptr,
            {},
            RHILoadAction::Clear
        );

        cmdList->SetViewport(RHIViewport{
            .x = 0, .y = 0,
            .width = fWidth, .height = fHeight,
            .minDepth = 0, .maxDepth = 1
        });

        renderer.Draw(*cmdList);

        cmdList->EndRenderPass();
        cmdList->Close();

        device->Submit(*cmdList, swapchain.get());
    }

    cmdList->WaitUntilCompleted();

    return 0;
}
