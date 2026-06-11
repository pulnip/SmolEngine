#pragma once

#include <span>
#include <vector>
#include "Canvas2D.hpp"
#include "GenericHandle.hpp"
#include "Primitives.hpp"
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

    class ShapeRenderer;

    class LineProxy final{
    public:
        using Handle = GenericHandle<LineRenderItem>;

    private:
        Handle handle = Handle::InvalidHandle();
        ShapeRenderer* renderer = nullptr;

    public:
        LineProxy() noexcept = default;
        ~LineProxy();
        SMOL_DECLARE_NON_COPYABLE(LineProxy)
        LineProxy(LineProxy&&) noexcept;
        LineProxy& operator=(LineProxy&&) noexcept;

        LineProxy(Handle handle, ShapeRenderer& renderer)
            : handle(handle), renderer(&renderer){}

        bool IsValid() const noexcept{
            auto isRendererValid = renderer != nullptr;
            auto isHandleValid = handle.IsValid();

            return isRendererValid && isHandleValid;
        }

        // handy forwading
        Points& GetRef() noexcept;
    };
}
