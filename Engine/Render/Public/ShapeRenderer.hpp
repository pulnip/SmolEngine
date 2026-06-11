#pragma once

#include "Canvas2D.hpp"
#include "LineProxy.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"
#include "SlotMap.hpp"

namespace Smol
{
    class ShapeRenderer final{
    public:
        using LineHandle = LineProxy::Handle;

    private:
        Canvas2DRAII canvas;

        SlotMap<LineRenderItem> lines;

    public:
        ShapeRenderer(RHIDevice&);
        ~ShapeRenderer();
        SMOL_DECLARE_PINNED(ShapeRenderer)

        [[nodiscard]]
        LineProxy BindLine(std::span<const Vec3>, Color color);

        // Notice. Draw at last
        void Draw(RHISwapchain&);

    private:
        friend class LineProxy;

        LineRenderItem& Get(LineHandle);
        void Unbind(LineHandle);
    };
}
