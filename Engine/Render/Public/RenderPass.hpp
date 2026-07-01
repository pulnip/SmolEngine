#pragma once

#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class View; class Scene;

    class RenderPass{
        SMOL_DECLARE_INTERFACE(RenderPass)

    public:
        virtual void Execute(RHICommandList&, const View&, const Scene&) = 0;
    };
}
