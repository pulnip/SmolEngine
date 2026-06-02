#pragma once

#include "InputAction.hpp"

namespace Smol
{
    // Input Manager for Single Context
    class IInputManager{
    public:
        using Callback = InputAction::Callback;
        using Handle = InputAction::Handle;

    public:
        // Notice! Not Guarantee the order in Single frame
        [[nodiscard]]
        virtual InputAction BindAction(StrView action, TriggerEvent, Callback&&) = 0;
        virtual void UnbindAction(Handle) = 0;
    };
}
