#pragma once

#include <span>
#include "Canvas2D.hpp"
#include "Component.hpp"
#include "LineProxy.hpp"

namespace Smol
{
    class LineRenderer: public TypedComponent<LineRenderer>{
        SMOL_COMPONENT_BODY(LineRenderer)

    private:
        std::vector<LineProxy> proxies;

    public:
        LineRenderer() = default;
        virtual ~LineRenderer() = default;
        SMOL_DECLARE_PINNED(LineRenderer)

        void Update(f32) override;

        // Write Discard mode
        void DrawLine(std::span<const Vec3>, Color color = Colors::Red);
    };
}
