#pragma once

#include "InputManager.hpp"
#include "MainLoop.hpp"
#include "Renderer.hpp"
#include "RHIFWD.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;
    class OS;

    class AppMainLoop: public MainLoop{
    private:
        InputManager inputManager;
        World world;
        Renderer renderer;

    public:
        AppMainLoop(const AppConfig&, OS&, RHIDevice&);

        bool Initialize() override;
        bool Update() override;
        bool Render(CommandListPool&) override;
        void Finalize() override;
    };
}
