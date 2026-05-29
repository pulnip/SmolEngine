#pragma once

#include "InputValue.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputModifier{
    public:
        SMOL_DECLARE_INTERFACE(InputModifier)

        virtual InputValue Modify(InputValue v) noexcept = 0;
    };
}
