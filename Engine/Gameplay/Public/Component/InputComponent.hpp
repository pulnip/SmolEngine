#pragma once

#include <unordered_map>
#include "Component.hpp"
#include "InputAction.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class IInputManager;
    class Pawn;

    class InputComponent: public TypedComponent<InputComponent>{
    private:
        IInputManager& manager;
        std::unordered_map<Pawn*, std::vector<InputAction>> actions;

    public:
        virtual ~InputComponent() = default;
        SMOL_DECLARE_PINNED(InputComponent)

        InputComponent(IInputManager& manager);

        void BindAction(StrView action, TriggerEvent, Pawn*, InputAction::Callback&&);
        void UnbindAction(Pawn* pawn);
    };
}
