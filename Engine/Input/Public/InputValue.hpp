#pragma once

#include "Primitives.hpp"
#include "Semantics.hpp"

namespace Smol
{
    struct InputValue final{
    private:
        Vec3 raw = zeros();

    public:
        constexpr InputValue() = default;
        ~InputValue() = default;
        SMOL_DECLARE_TRANSFERABLE_NOEXCEPT(InputValue)

        constexpr InputValue(Vec3 raw)
            : raw(raw) {}
        constexpr InputValue(f32 x, f32 y, f32 z)
            : raw(Vec3(x, y, z)) {}

        inline bool GetDigital() const noexcept{
            constexpr f32 eps = 1e-3;
            auto val = raw.x < 0 ? -raw.x : raw.x;

            return val > eps;
        }
        inline f32 GetAxis1D() const noexcept{
            return raw.x;
        }
        inline Vec2 GetAxis2D() const noexcept{
            return Vec2(raw);
        }
        inline Vec3 GetAxis3D() const noexcept{
            return raw;
        }

        inline InputValue& operator+=(InputValue other) noexcept{
            raw += other.GetAxis3D();

            return *this;
        }
    };
}
