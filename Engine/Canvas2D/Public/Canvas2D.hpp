#pragma once

#include <span>
#include "Primitives.hpp"
#include "RHIFWD.hpp"
#include "Semantics.hpp"

namespace Smol
{
    // xyzw = rgba
    using Color = Vec4;

    namespace Colors{
        constexpr Color Black   = {0, 0, 0, 1};
        constexpr Color Red     = {1, 0, 0, 1};
        constexpr Color Green   = {0, 1, 0, 1};
        constexpr Color Blue    = {0, 0, 1, 1};
        constexpr Color Cyan    = {0, 1, 1, 1};
        constexpr Color Magenta = {1, 0, 1, 1};
        constexpr Color Yellow  = {1, 1, 0, 1};
        constexpr Color White   = {1, 1, 1, 1};
    }

    struct StrokeStyle{
        Color color = Colors::Red;
        f32 width = 1.0f;
        std::span<const f32> dashPattern = {};
        bool screenSpaceWidth = true;
    };

    struct TextStyle{
        Color color = Colors::Red;
        f32 fontSize = 15.0f;
        Size2D size;
    };

    class Canvas2D{
    public:
        SMOL_DECLARE_INTERFACE(Canvas2D)

        virtual void Begin(RHISwapchain&) = 0;
        virtual void End() = 0;

        virtual void SetTransform(Transform2D) = 0;

        virtual void Line(Vec2 a, Vec2 b, const StrokeStyle&) = 0;
        virtual void Polyline(std::span<const Vec2>, const StrokeStyle&) = 0;
        virtual void Circle(Vec2 center, f32 r, const StrokeStyle&) = 0;
        virtual void Rect(Vec2 min, Vec2 max, const StrokeStyle&) = 0;
        virtual void Text(StrView utf8, Vec2 pos, const TextStyle&) = 0;
    };

    using Canvas2DRAII = RAII<Canvas2D>;

    // each platform should implement this function
    Canvas2DRAII CreateCanvas(RHIDevice&);
}
