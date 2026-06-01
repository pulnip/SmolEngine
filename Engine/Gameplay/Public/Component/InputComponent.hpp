#pragma once

#include "Component.hpp"
#include "InputAction.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputComponent: public TypedComponent<InputComponent>{
    private:
        std::vector<InputAction> actions;

    public:
        InputComponent() = default;
        virtual ~InputComponent() = default;
        SMOL_DECLARE_MOVE_ONLY(InputComponent)
    };
}
