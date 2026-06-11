#pragma once

#include <span>
#include <vector>
#include "Canvas2D.hpp"
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    using Point = Vec3;
    using Points = std::vector<Point>;

    struct LineRenderItem{
        Points points;
        StrokeStyle style;

        explicit LineRenderItem(std::span<const Point> points, Color color)
            : points(std::from_range, points)
            , style(StrokeStyle{
                .color = color,
                .width = 1.0f,
                .dashPattern = {},
                .screenSpaceWidth = true
            }){}
    };

    class ShapeRenderer final{
    private:
        Canvas2DRAII canvas;

        // Write-Discard buffer
        std::vector<LineRenderItem> lines;

    public:
        ShapeRenderer(RHIDevice&);
        ~ShapeRenderer();
        SMOL_DECLARE_PINNED(ShapeRenderer)

        void SubmitLine(std::span<const Vec3>, Color color);

        // Notice. Draw at last
        void Draw(RHISwapchain&);
    };
}
