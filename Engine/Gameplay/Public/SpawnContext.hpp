#pragma once

#include "DOM.hpp"

namespace Smol
{
    class IInputManager;
    class RHIDevice;
    class SpriteRenderer;

    struct SpawnContext{
        static IInputManager& mockInputManager();

        const DOM::Value& dom = {};
        IInputManager& inputManager = mockInputManager();
        // TODO. mock device?
        RHIDevice* device = nullptr;
        // TODO. mock renderer?
        SpriteRenderer* spriteRenderer = nullptr;
    };
}
