#pragma once

#include "InputManager.hpp"
#include "MainLoop.hpp"
#include "ResourceRegistry.hpp"
#include "PostRenderer.hpp"
#include "RHIFWD.hpp"
#include "SpriteRenderer.hpp"
#include "Timer.hpp"
#include "UIRenderer.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;
    class OS;

    class AppMainLoop: public MainLoop{
    private:
        ResourceRegistry resourceRegistry;

        InputManager inputManager;
        SpriteRenderer spriteRenderer;
        PostRenderer postRenderer;
        bool showRain = false;
        i32 colorInversion = false;
        UIRenderer uiRenderer;

        World world;

        Timer timer;

        // TODO.
        RHITextureRAII scene;

    public:
        AppMainLoop(const AppConfig&, OS&, RHIDevice&);

        bool Initialize() override;
        bool Update() override;
        bool Render(CommandListPool&, RHISwapchain&) override;
        void Finalize() override;
    };
}
