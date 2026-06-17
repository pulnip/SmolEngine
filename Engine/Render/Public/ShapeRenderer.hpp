#pragma once

#include <span>
#include <vector>
#include "Canvas2D.hpp"
#include "Geometry/Overlap2D.hpp"
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
    struct TextRenderItem{
        Str text;
        Vec2 pos; // screen coord
        TextStyle style;

        explicit TextRenderItem(StrView text, Vec2 pos, Color color, f32 fontSize)
            : text(text)
            , pos(pos)
            , style(TextStyle{
                .color = color,
                .fontSize = fontSize,
                // Notice. WORD_WRAPPING = NO_WRAP
                .size = {0, 0}
            }){}
    };

    class ShapeRenderer final{
    private:
        Canvas2DRAII canvas;

        // Write-Discard buffer
        std::vector<LineRenderItem> lines;
        std::vector<TextRenderItem> texts;

    public:
        ShapeRenderer(RHIDevice&);
        ~ShapeRenderer();
        SMOL_DECLARE_PINNED(ShapeRenderer)

        void SubmitLine(std::span<const Vec3>, Color color);
        void SubmitText(StrView text, Vec2 pos, Color color, f32 fontSize);

        // Debug Collider
        void SubmitOBB2D(OBB2D, Color color);

        // Notice. Draw at last
        void Draw(RHISwapchain&);
    };
}
