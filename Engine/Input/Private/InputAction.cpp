#include "InputAction.hpp"
#include "IInputManager.hpp"

namespace Smol
{
    InputAction::InputAction(InputAction&& other) noexcept
        : handle(other.handle)
        , manager(other.manager)
    {
        other.handle = Handle::InvalidHandle();
        other.manager = nullptr;
    }
    InputAction& InputAction::operator=(InputAction&& other) noexcept{
        handle = other.handle;
        manager = other.manager;

        other.handle = Handle::InvalidHandle();
        other.manager = nullptr;

        return *this;
    }

    InputAction::~InputAction(){
        auto isManagerValid = manager != nullptr;
        auto isHandleValid = handle.IsValid();

        if(isManagerValid && isHandleValid){
            manager->UnbindAction(handle);

            handle = Handle::InvalidHandle();
            manager = nullptr;
        }
    }
}
