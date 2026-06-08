#include "Actor.hpp"
#include "InputComponent.hpp"
#include "IInputManager.hpp"
#include "World.hpp"

namespace Smol
{
    SMOL_COMPONENT(InputComponent)
    SMOL_COMPONENT_END(InputComponent)

    void InputComponent::BindAction(
        StrView action,
        TriggerEvent event,
        Pawn* pawn,
        InputAction::Callback&& callback
    ){
        auto manager = owner->GetWorld()->GetInputManager();

        auto handle = manager->BindAction(
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
