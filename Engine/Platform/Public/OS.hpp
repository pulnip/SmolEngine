#pragma once

#include "CoreFWD.hpp"
#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class MainLoop;

    struct RuntimeConfig;
    // type itself is context-dependent
    // in Windows, NativeWindowHandle = HWND
    // in macOS, NativeWindowHandle = CA::MetalLayer*
    // otherwise, nullptr
    using NativeWindowHandle = void*;

    class OS{
    private:
        class Impl;
        RAII<Impl> impl;

        static OS* singleton;

    public:
        OS(const RuntimeConfig&);
        ~OS();
        SMOL_DECLARE_PINNED(OS)

        void Run(MainLoop&);

        inline static OS* Get(){ return singleton; }

        u32 GetWidth() const;
        u32 GetHeight() const;

        NativeWindowHandle GetWindow() const;
    };

    // direct access to ptr is unsafe, but handy helper
    #define OS_ *OS::Get()
}