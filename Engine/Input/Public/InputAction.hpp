#pragma once

#include <functional>
#include "GenericHandle.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputManager;

    // RAII for ActionHandle
    class InputAction final{
    public:
        using Callback = std::function<void()>;
        using Handle = GenericHandle<Callback>;

    private:
        Handle handle = Handle::InvalidHandle();
        InputManager* manager = nullptr;

    public:
        InputAction() noexcept = default;
        ~InputAction();
        SMOL_DECLARE_NON_COPYABLE(InputAction)
        InputAction(InputAction&& other) noexcept;
        InputAction& operator=(InputAction&&);

        InputAction(Handle handle, InputManager* manager)
            : handle(handle), manager(manager){}
    };
}
