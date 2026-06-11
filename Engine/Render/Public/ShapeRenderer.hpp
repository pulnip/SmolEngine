#pragma once

#include "Canvas2D.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    class ShapeRenderer final{
    private:
        Canvas2DRAII canvas;

    public:
        ShapeRenderer(RHIDevice&);
        ~ShapeRenderer();
        SMOL_DECLARE_PINNED(ShapeRenderer)

        // Notice. Draw at last
        void Draw(RHISwapchain&);
    };
}
