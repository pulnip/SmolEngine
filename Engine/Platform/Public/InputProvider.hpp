#pragma once

#include <bitset>
#include "KeyCode.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputProvider{
    protected:
        std::bitset<NUM_KEY> currentKeys;
        std::bitset<NUM_KEY> previousKeys;

    public:
        SMOL_DECLARE_INTERFACE(InputProvider)

        virtual void Poll() = 0;

        // only check current state
        bool IsKeyDown(KeyCode keyCode) const noexcept;
        // combination of previous state and current state
        KeyState GetKeyState(KeyCode) const noexcept;
        // reset keystate to default
        void Reset() noexcept;
    };
}
