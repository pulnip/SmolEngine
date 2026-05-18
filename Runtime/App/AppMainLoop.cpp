#include "AppConfig.hpp"
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

        return false;
    }

    bool AppMainLoop::Render(RHIDevice&){
        return true;
    }

    void AppMainLoop::Finalize(){

    }
}
