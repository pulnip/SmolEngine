#pragma once

#include "InputProvider.hpp"

namespace Smol
{
    class MockInputProvider final: public InputProvider{
    private:
        std::bitset<NUM_KEY> transitionKeys;

    public:
        MockInputProvider() = default;
        ~MockInputProvider() = default;

        void Poll() override;
        void Reset();

        // simulate keyboard
        void PressKey(KeyCode);
        void ReleaseKey(KeyCode);
    };
}
