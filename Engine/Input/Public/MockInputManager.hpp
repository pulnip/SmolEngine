#pragma once

#include "IInputManager.hpp"

namespace Smol
{
    class MockInputManager: public IInputManager{
    public:
        [[nodiscard]]
        InputAction BindAction(StrView action, TriggerEvent, Callback&&) override{
            return {};
        }
        void UnbindAction(Handle) override{}
    };
}
