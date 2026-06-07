#pragma once

#include "ImmediateResourceLoader.hpp"
#include "InputManager.hpp"
#include "MainLoop.hpp"
#include "Resource.hpp"
#include "ResourceManager.hpp"
#include "RHIFWD.hpp"
#include "SpriteRenderer.hpp"
#include "Timer.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;
    class OS;

    class AppMainLoop: public MainLoop{
    private:
        ImmediateResourceLoader resourceLoader;
        ResourceManager<SpriteResource> spriteManager;

        InputManager inputManager;
        SpriteRenderer spriteRenderer;

        World world;

        Timer timer;

    public:
        AppMainLoop(const AppConfig&, OS&, RHIDevice&);

        bool Initialize() override;
        bool Update() override;
        bool Render(CommandListPool&, RHISwapchain&) override;
        void Finalize() override;
    };
}
