#pragma once

#include <variant>
#include "Primitives.hpp"
#include "Geometry/Overlap2D.hpp"

namespace Smol
{
    struct RectCollider{
        Transform2D transform{
            .position = Vec2{0, 0},
            .theta = 0.0f,
            .scale = Vec2{1, 1}
        };

        operator OBB2D() const noexcept{
            return makeOBB2D(transform);
        }
    };

    using Shape2D = std::variant<
        // Disk,
        RectCollider
    >;

    inline constexpr auto Collide(const RectCollider& r0, const RectCollider& r1) noexcept{
        return OverlapOBB2DEx(r0, r1);
    }

    inline OverlapResult2D Collide(const Shape2D& c0, const Shape2D& c1){
        return std::visit(
            [](const auto& c0, const auto& c1){
                return Collide(c0, c1);
            },
            c0, c1
        );
    }
}
