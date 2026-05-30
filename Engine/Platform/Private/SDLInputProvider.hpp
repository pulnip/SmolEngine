#pragma once

#include <SDL3/SDL_events.h>
#include "InputProvider.hpp"

namespace Smol
{
    class SDLInputProvider final: public InputProvider{
    private:
        const bool* heldState;

    public:
        SDLInputProvider();
        ~SDLInputProvider();

        bool IsKeyDown(KeyCode) const noexcept override;

        // Call Before Event Process
        void NewFrame() noexcept{
            InputProvider::ConsumeEdge();
        }
        void Reset() noexcept{
            InputProvider::ConsumeEdge();
        }

        // Process Event
        void OnPlatformEvent(const SDL_Event& event) noexcept;
    };
}
