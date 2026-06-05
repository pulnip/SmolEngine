#pragma once

#include <filesystem>
#include "DOM.hpp"

namespace Smol
{
    class IInputManager;
    class RHIDevice;
    class SpriteRenderer;
    class World;

    struct SpawnContext{
        static IInputManager& mockInputManager();

        const DOM::Value& dom = {};
        const std::filesystem::path& contentRoot = {};

        IInputManager& inputManager = mockInputManager();
        // TODO. mock device?
        RHIDevice* device = nullptr;
        // TODO. mock renderer?
        SpriteRenderer* spriteRenderer = nullptr;
        // TODO. mock world?
        World* world = nullptr;
    };
}
