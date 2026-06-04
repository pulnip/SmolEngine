#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "CommandListPool.hpp"
#include "InputConfig.hpp"
#include "InputModifier.hpp"
#include "OS.hpp"
#include "RHICommandList.hpp"
#include "RHIDevice.hpp"
#include "RHISwapchain.hpp"

namespace Smol
{
    AppMainLoop::AppMainLoop(
        const AppConfig& config,
        OS& os,
        RHIDevice& device
    )
        // TODO. use toml later
        : inputManager(InputConfig{
            .mappings = {
                ActionInfo{
                    .name = "Move",
                    .mappings = {
                        KeyBinding{
                            .keyCode = KeyCode::A,
                            .modifiers = {
                                ScaleModifier(10.0f),
                                NegateModifier(true, false, false)
                            }
                        },
                        KeyBinding{
                            .keyCode = KeyCode::D,
                            .modifiers = {
                                ScaleModifier(10.0f),
                            }
                        }
                    }
                },
                ActionInfo{
                    .name = "Jump",
                    .mappings = {
                        KeyBinding{
                            .keyCode = KeyCode::W
                        },
                        KeyBinding{
                            .keyCode = KeyCode::Space
                        }
                    }
                }
            }
        }, &os.GetInputProvider())
        , world("")
        , spriteRenderer(device)
    {

    }

    bool AppMainLoop::Initialize(){

        return true;
    }

    bool AppMainLoop::Update(){
        world.Update(0.0f);

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
