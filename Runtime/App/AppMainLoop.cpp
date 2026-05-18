#include "AppConfig.hpp"
#include "CommandListPool.hpp"
#include "RHIDevice.hpp"
#include "AppMainLoop.hpp"
#include "Renderer.hpp"

namespace Smol
{
    AppMainLoop::AppMainLoop(
        const AppConfig& config,
        RHIDevice& device
    )
        : renderer(device)
        , world("")
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
