#pragma once

#include "InputManager.hpp"
#include "MainLoop.hpp"
#include "RHIFWD.hpp"
#include "SpriteRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;
    class OS;

    class AppMainLoop: public MainLoop{
    private:
        InputManager inputManager;
        World world;
        SpriteRenderer spriteRenderer;

    public:
        AppMainLoop(const AppConfig&, OS&, RHIDevice&);

        bool Initialize() override;
        bool Update() override;
        bool Render(CommandListPool&, RHISwapchain&) override;
        void Finalize() override;
    };
}
