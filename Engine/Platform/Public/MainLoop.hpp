#pragma once

#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class MainLoop{
    public:
        SMOL_DECLARE_INTERFACE(MainLoop)

        virtual bool Initialize(){ return true; }
        virtual bool Update() = 0;
        virtual bool Render(CommandListPool&) = 0;
        virtual void Finalize(){}
    };
}
