#pragma once

#include <span>
#include "Canvas2D.hpp"
#include "Component.hpp"

namespace Smol
{
    class LineRenderer: public TypedComponent<LineRenderer>{
        SMOL_COMPONENT_BODY(LineRenderer)

    public:
        LineRenderer() = default;
        virtual ~LineRenderer() = default;
        SMOL_DECLARE_PINNED(LineRenderer)

        // Write Discard mode
        // Notice. points must be located in World coord
        void DrawLine(std::span<const Vec3>, Color color = Colors::Red);

        // TODO. move to another component
        void DrawText(StrView, Vec2 pos, Color color = Colors::Red, f32 fontSize = 15.0f);
    };
}
