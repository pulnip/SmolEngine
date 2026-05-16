#pragma once

#include "MainLoop.hpp"
#include "World.hpp"

namespace Smol
{
    struct AppConfig;

    class AppMainLoop: public MainLoop{
    private:
        World world;

    public:
        AppMainLoop(const AppConfig&);

        bool Initialize() override;
        bool Update() override;
        void Finalize() override;
    };
}
