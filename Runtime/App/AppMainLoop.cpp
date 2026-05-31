#include "AppConfig.hpp"
#include "AppMainLoop.hpp"
#include "CommandListPool.hpp"
#include "InputConfig.hpp"
#include "InputModifier.hpp"
#include "OS.hpp"
#include "RHIDevice.hpp"
#include "Renderer.hpp"

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
        }, os.GetInputProvider())
        , world("")
        , renderer(device)
    {

    }

    bool AppMainLoop::Initialize(){

        return true;
    }

    bool AppMainLoop::Update(){
        world.Update(0.0f);

        return true;
    }

    bool AppMainLoop::Render(CommandListPool&){
        return true;
    }

    void AppMainLoop::Finalize(){

    }
}
