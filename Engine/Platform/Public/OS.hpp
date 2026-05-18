#pragma once

#include "CoreFWD.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class MainLoop;

    struct RuntimeConfig;

    class OS{
    private:
        class Impl;
        RAII<Impl> impl;

        static OS* singleton;

    public:
        OS(const RuntimeConfig&, RHIDevice&);
        ~OS();
        SMOL_DECLARE_PINNED(OS)

        void Run(MainLoop&, RHIDevice&);

        inline static OS* Get(){ return singleton; }

        u32 GetWidth() const;
        u32 GetHeight() const;
    };

    // direct access to ptr is unsafe, but handy helper
    #define OS_ *OS::Get()
}