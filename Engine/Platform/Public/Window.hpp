#pragma once

#include "CoreFWD.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct WindowConfig;
    class InputProvider;

    class Window final{
    private:
        class Impl;
        RAII<Impl> impl;

    public:
        Window(const WindowConfig&);
        ~Window();
        SMOL_DECLARE_PINNED(Window)

        bool ProcessEvents();

        void* GetWindow() const noexcept;
        u32 GetWidth() const noexcept;
        u32 GetHeight() const noexcept;
        Size2D GetSize() const noexcept;

        const InputProvider* GetInputProvider() const noexcept;
    };
}
