#include "InputComponent.hpp"
#include "InputManager.hpp"

namespace Smol
{
    InputComponent::InputComponent(InputManager& manager)
        : manager(manager) {}

    void InputComponent::bindAction(
        StrView action,
        TriggerEvent event,
        InputAction::Callback&& callback
    ){
        auto handle = manager.BindAction(
            action,
            event,
            std::move(callback)
        );
        actions.emplace_back(std::move(handle));
    }
}
