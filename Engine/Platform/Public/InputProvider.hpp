#pragma once

#include <bitset>
#include "KeyCode.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class InputProvider{
    private:
        std::bitset<NUM_KEY> pressedState;
        std::bitset<NUM_KEY> releasedState;

    public:
        SMOL_DECLARE_INTERFACE(InputProvider)

        // Call After All Event Process Done
        virtual bool IsKeyDown(KeyCode) const noexcept = 0;

        // true if Rising Edge Exist
        bool IsKeyPressed(KeyCode keyCode) const noexcept{
            auto ordKey = static_cast<usize>(keyCode);

            return pressedState.test(ordKey);
        }
        // true if Falling Edge Exist
        bool IsKeyReleased(KeyCode keyCode) const noexcept{
            auto ordKey = static_cast<usize>(keyCode);

            return releasedState.test(ordKey);
        }

    protected:
        void ConsumeEdge() noexcept{
            pressedState.reset();
            releasedState.reset();
        }

        void PressKey(KeyCode keyCode) noexcept{
            auto ordKey = static_cast<usize>(keyCode);

            pressedState.set(ordKey);
        }

        void ReleaseKey(KeyCode keyCode) noexcept{
            auto ordKey = static_cast<usize>(keyCode);

            releasedState.set(ordKey);
        }
    };
}
