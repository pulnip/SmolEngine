#include "InputComponent.hpp"
#include "InputManager.hpp"

namespace Smol
{
    InputComponent::InputComponent(InputManager& manager)
        : manager(manager) {}

    void InputComponent::BindAction(
        StrView action,
        TriggerEvent event,
        Pawn* pawn,
        InputAction::Callback&& callback
    ){
        auto handle = manager.BindAction(
            action,
            event,
            std::move(callback)
        );
        actions[pawn].emplace_back(std::move(handle));
    }

    void InputComponent::UnbindAction(Pawn* pawn){
        actions.erase(pawn);
    }
}
