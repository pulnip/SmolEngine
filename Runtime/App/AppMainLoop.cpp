#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "CommandListPool.hpp"
#include "OS.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHITexture.hpp"
#include "RHISwapchain.hpp"
#include "ResourceManager.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    AppMainLoop::AppMainLoop(
        const AppConfig& config,
        OS& os,
        RHIDevice& device
    )
        : resourceLoader(device, config.project.content_root)
        , spriteManager(resourceLoader)
        // TODO. use toml later
        , inputManager(
            loadTomlFile<InputConfig>(config.defaultInputPath()),
            &os.GetInputProvider()
        )
        , spriteRenderer(device, spriteManager)
        , world(
            parseTomlFile(config.startupScenePath()),
            SpawnContext{
                .spriteManager = &spriteManager,
                .inputManager = inputManager,
                .device = &device,
                .spriteRenderer = &spriteRenderer,
                .world = &world
            }
        )
    {
        spriteManager.DrainCompletions();
    }

    bool AppMainLoop::Initialize(){
        timer.Reset();

        return true;
    }

    bool AppMainLoop::Update(){
        timer.NewFrame();
        auto deltaTime = timer.GetDeltaTime();

        inputManager.NewFrame();
        world.Update(static_cast<f32>(deltaTime));

        return true;
    }

    bool AppMainLoop::Render(CommandListPool& pool, RHISwapchain& swapchain){
        // for single thread model, use single cmdList.
        auto& cmdList = pool.Acquire();
        cmdList.Begin();

        // TODO. use integrated Renderer class
        // Begin RenderPass for backbuffer
        RHIClearColor backbufferClearColor{.v = {
            0.5f, 0.5f, 0.5f, 1.0f
        }};
        cmdList.BeginRenderPass(swapchain,
            backbufferClearColor,
            nullptr,
            {},
            RHILoadAction::Clear
        );
        cmdList.SetViewport(RHIViewport{
            .x = 0, .y = 0,
            // fill all backbuffer
            .width = static_cast<f32>(swapchain.GetWidth()),
            .height = static_cast<f32>(swapchain.GetHeight()),
            .minDepth = 0, .maxDepth = 1
        });
        spriteRenderer.Draw(cmdList);

        cmdList.EndRenderPass();
        // End RenderPass for backbuffer

        cmdList.Close();
        return true;
    }

    void AppMainLoop::Finalize(){

    }
}
