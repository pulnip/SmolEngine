#pragma once

#include "Canvas2D.hpp"

namespace Smol
{
    class D2DCanvas final: public Canvas2D{
    private:
        class Impl;
        RAII<Impl> impl;

    public:
        D2DCanvas(RHIDevice&);
        ~D2DCanvas();

        void Begin(RHISwapchain&) override;
        void End() override;

        void SetTransform(Transform2D) override;

        void Line(Vec2 a, Vec2 b, const StrokeStyle&) override;
        void Polyline(std::span<const Vec2>, const StrokeStyle&) override;
        void Circle(Vec2 center, f32 r, const StrokeStyle&) override;
        void Rect(Vec2 min, Vec2 max, const StrokeStyle&) override;
        void Text(StrView utf8, Vec2 pos, const TextStyle&) override;
    };
}
