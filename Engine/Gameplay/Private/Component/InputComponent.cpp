#include "InputComponent.hpp"
#include "IInputManager.hpp"
#include "SpawnContext.hpp"

namespace Smol
{
    SMOL_COMPONENT(InputComponent)
    SMOL_COMPONENT_END(InputComponent)

    InputComponent::InputComponent(const SpawnContext& ctx)
        : manager(ctx.inputManager) {}

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
