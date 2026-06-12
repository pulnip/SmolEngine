#pragma once

#include "InputManager.hpp"
#include "MainLoop.hpp"
#include "ResourceRegistry.hpp"
#include "RHIFWD.hpp"
#include "ShapeRenderer.hpp"
#include "SpriteRenderer.hpp"
#include "Timer.hpp"
#include "Widget.hpp"
#include "WidgetRenderer.hpp"
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
        ShapeRenderer shapeRenderer;
        WidgetRenderer widgetRenderer;

        Widget widget;

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
