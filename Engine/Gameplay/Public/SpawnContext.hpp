#pragma once

#include "DOM.hpp"

namespace Smol
{
    class IInputManager;

    struct SpawnContext{
        static IInputManager& mockInputManager();

        const DOM::Value& dom = {};
        IInputManager& inputManager = mockInputManager();
    };
}
