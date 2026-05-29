#pragma once

#include "InputProvider.hpp"

namespace Smol
{
    class SDLInputProvider final: public InputProvider{
    private:
        const bool* transitionKeys;

    public:
        SDLInputProvider();

        void Poll() override;
    };
}
