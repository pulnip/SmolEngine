#pragma once

#include <array>
#include "GeoUtil.hpp"
#include "Geometry/GJK.hpp"
#include "LinearAlgebra.hpp"
#include "Primitives.hpp"

namespace Smol
{
    struct Ball2D{
        Vec2 center{.x = 0.0f, .y = 0.0f};
        f32 radius = 1.0f;
    };
    using Disk = Ball2D;
    using Circle = Ball2D;

    inline constexpr auto OverlapBall2D(const Ball2D b0, const Ball2D b1) noexcept{
        const auto diff = b1.center - b0.center;
        const auto r2 = b0.radius + b1.radius;

        return normSquared(diff) <= r2*r2;
    }

    struct AABB2D{
        Vec2 center;
        Vec2 halfScale;
    };
    inline constexpr auto makeAABB2D(Transform2D t) noexcept{
        return AABB2D{
            .center = t.position,
            .halfScale = 0.5f * t.scale
        };
    }

    inline constexpr auto OverlapAABB2D(
        const AABB2D b0, const AABB2D b1,
        const f32 epsilon = 0.0f
    ) noexcept{
        const auto overlap_x = overlap(
            b0.center.x - b0.halfScale.x, b0.center.x + b0.halfScale.x,
            b1.center.x - b1.halfScale.x, b1.center.x + b1.halfScale.x,
            epsilon
        );
        const auto overlap_y = overlap(
            b0.center.y - b0.halfScale.y, b0.center.y + b0.halfScale.y,
            b1.center.y - b1.halfScale.y, b1.center.y + b1.halfScale.y,
            epsilon
        );

        return overlap_x && overlap_y;
    }

    struct OBB2D{
        Vec2 center{.x = 0.0f, .y = 0.0f};
        std::array<Vec2, 2> halfAxes = {
            Vec2{.x = 0.5f, .y = 0.0f},
            Vec2{.x = 0.0f, .y = 0.5f}
        };
    };
    inline constexpr auto makeOBB2D(Transform2D t) noexcept{
        return OBB2D{
            .center = t.position,
            .halfAxes = {
                (0.5f * t.scale.x) * right(t.theta),
                (0.5f * t.scale.y) * up(t.theta)
            }
        };
    }

    namespace detail{
        inline constexpr auto project(const Vec2 axis, const OBB2D b){
            return Projected{
                .center = dot(axis, b.center),
                .radius =
                    std::abs(dot(axis, b.halfAxes[0])) +
                    std::abs(dot(axis, b.halfAxes[1]))
            };
        }
    }

    inline constexpr bool OverlapOBB2D(
        const OBB2D b0, const OBB2D b1,
        const f32 epsilon = 0.0f
    ){
        using detail::project;

        const std::array axes = {
            perp(b0.halfAxes[0]), perp(b0.halfAxes[1]),
            perp(b1.halfAxes[0]), perp(b1.halfAxes[1])
        };

        for(const auto axis: axes){
            auto p0 = project(axis, b0);
            auto p1 = project(axis, b1);

            if(!Overlap(p0, p1, epsilon))
                return false;
        }

        return true;
    }

    namespace detail{
        inline constexpr Vec2 support(const OBB2D b, const Vec2 dir){
            return b.center + (
                dot(dir, b.halfAxes[0]) >= 0.0f ?
                    b.halfAxes[0] : -b.halfAxes[0]
            ) + (
                dot(dir, b.halfAxes[1]) > 0.0f ?
                    b.halfAxes[1] : -b.halfAxes[1]
            );
        }
    }

    struct SweepResult2D{
        bool isHit = false;
        bool startPenetrating = false;
        // 0~1
        f32 time;
        Vec2 normal;
        // position when enter
        Vec2 location;
        Vec2 impactPoint;
        f32 distance = 0.0f;
    };

    inline constexpr SweepResult2D SweepOBB2D(
        const OBB2D b0, const OBB2D b1,
        // b1에 대한 b0의 상대 dPos
        const Vec2 move,
        const f32 epsilon = 0.0f
    ){
        using detail::project;

        const std::array axes = {
            perp(b0.halfAxes[0]), perp(b0.halfAxes[1]),
            perp(b1.halfAxes[0]), perp(b1.halfAxes[1])
        };

        SweepResult2D r{
            .isHit = false
        };

        f32 tEnter = 0.0f;
        f32 tLeave = 1.0f;

        i32 hitIndex = -1;
        Vec2 hitAxis;

        for(i32 i=0; i<axes.size(); ++i){
            const Vec2 axis = axes[i];
            const auto p0 = project(axis, b0);
            const auto p1 = project(axis, b1);

            const auto d = p1.center - p0.center;
            const auto R = p0.radius + p1.radius + epsilon;
            const auto speed = dot(move, axis);

            const f32 e0 = d - R;
            const f32 e1 = d + R;

            // relative velocity is almost 0
            if(abs(speed) < 1e-8f){
                if(e0 > 0.0f || e1 < 0.0f)
                    return r;
                continue;
            }

            auto t0 = e0 / speed;
            auto t1 = e1 / speed;
            if(t0 > t1) std::swap(t0, t1);

            if(t0 > tEnter){
                tEnter = t0;
                hitIndex = i;
                hitAxis = axis;
            }
            if(t1 < tLeave){
                tLeave = t1;
            }

            if(tEnter > tLeave)
                return r;
        }

        r.isHit = true;

        // already overlap
        if(hitIndex < 0 || tEnter <= 0.0f){
            r.startPenetrating = true;
            r.time = 0.0f;

            return r;
        }

        r.time = tEnter;
        r.normal = dot(hitAxis, b0.center - b1.center) >= 0.0f ?
            hitAxis : -hitAxis;
        r.distance = norm(move) * r.time;

        using detail::support;

        const bool ownedByB0 = (hitIndex <= 1);
        r.impactPoint = ownedByB0 ?
            support(b1, r.normal) : support(b0, -r.normal);

        return r;
    }
}
