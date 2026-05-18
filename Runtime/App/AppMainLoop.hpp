#pragma once

#include "MainLoop.hpp"
#include "Renderer.hpp"
#include "RHIFWD.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;

    class AppMainLoop: public MainLoop{
    private:
        Renderer renderer;
        World world;

    public:
        AppMainLoop(const AppConfig&, RHIDevice&);

        bool Initialize() override;
        bool Update() override;
        bool Render(CommandListPool&) override;
        void Finalize() override;
    };
}
