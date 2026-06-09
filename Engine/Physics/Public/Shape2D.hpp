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

    inline constexpr auto Collide(const RectCollider& r0, const RectCollider& r1, Vec2 move) noexcept{
        return SweepOBB2D(r0, r1, move);
    }

    inline SweepResult2D Collide(const Shape2D& c0, const Shape2D& c1, Vec2 move){
        return std::visit([move](const auto& s0, const auto& s1){
            return Collide(s0, s1, move);
        }, c0, c1);
    }
}
