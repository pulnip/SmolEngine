#include "AppConfig.hpp"
#include "AppMainLoop.hpp"

namespace Smol
{
    AppMainLoop::AppMainLoop(const AppConfig& config)
        : world("")
    {

    }

    bool AppMainLoop::Initialize(){

        return true;
    }

    bool AppMainLoop::Update(){
        world.Update(0.0f);

        return false;
    }

    void AppMainLoop::Finalize(){

    }
}
