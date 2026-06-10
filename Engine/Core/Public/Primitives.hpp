#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <format>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include "Assert.hpp"

namespace Smol
{
	using i8  = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
    using i64 = std::int64_t;

	using u8  = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using usize = std::size_t;
    using isize = std::ptrdiff_t;

    using f32 = float;
    using f64 = double;

    using CStr = const char*;
    using Str = std::string;
    using StrView = std::string_view;

    template<typename T>
    using RAII = std::unique_ptr<T>;

    struct Vec3;
    struct Vec4;

    // column vector
    struct Vec2{
        f32 x = 0.0f, y = 0.0f;

        constexpr auto& operator[](this auto& self, usize i) noexcept{
            switch(i){
            case 0: return self.x;
            case 1: return self.y;
            default:
                SMOL_ASSERT(false, "Vec2 index out of range");
                std::unreachable();
            }
        }

        constexpr operator Vec3() const noexcept;
        constexpr operator Vec4() const noexcept;

        constexpr Vec2 yx() const noexcept{
            return {
                .x = y,
                .y = x
            };
        }
    };
#ifdef __cpp_lib_is_implicit_lifetime
    static_assert(std::is_implicit_lifetime_v<Vec2>);
#else
    static_assert(std::is_trivially_copyable_v<Vec2>);
#endif
    static_assert(std::is_standard_layout_v<Vec2>);

    // column vector
    struct Vec3{
        f32 x = 0.0f, y = 0.0f, z = 0.0f;

        constexpr auto& operator[](this auto& self, usize i) noexcept{
            switch(i){
            case 0: return self.x;
            case 1: return self.y;
            case 2: return self.z;
            default:
                SMOL_ASSERT(false, "Vec3 index out of range");
                std::unreachable();
            }
        }

        constexpr explicit operator Vec2() const noexcept{
            return {x, y};
        }
        constexpr operator Vec4() const noexcept;

        constexpr Vec3 xyz() const noexcept{
            return *this;
        }
        constexpr Vec3 xzy() const noexcept{
            return {
                .x = x,
                .y = z,
                .z = y
            };
        }
        constexpr Vec3 yxz() const noexcept{
            return {
                .x = y,
                .y = x,
                .z = z
            };
        }
        constexpr Vec3 yzx() const noexcept{
            return {
                .x = y,
                .y = z,
                .z = x
            };
        }
        constexpr Vec3 zxy() const noexcept{
            return {
                .x = z,
                .y = x,
                .z = y
            };
        }
        constexpr Vec3 zyx() const noexcept{
            return {
                .x = z,
                .y = y,
                .z = x
            };
        }
    };
#ifdef __cpp_lib_is_implicit_lifetime
    static_assert(std::is_implicit_lifetime_v<Vec3>);
#else
    static_assert(std::is_trivially_copyable_v<Vec3>);
#endif
    static_assert(std::is_standard_layout_v<Vec3>);

    // column vector
    struct Vec4{
        f32 x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

        constexpr auto& operator[](this auto& self, usize i) noexcept{
            switch(i){
            case 0: return self.x;
            case 1: return self.y;
            case 2: return self.z;
            case 3: return self.w;
            default:
                SMOL_ASSERT(false, "Vec4 index out of range");
                std::unreachable();
            }
        }

        constexpr explicit operator Vec2() const noexcept{
            return {x, y};
        }
        constexpr explicit operator Vec3() const noexcept{
            return {x, y, z};
        }
    };
#ifdef __cpp_lib_is_implicit_lifetime
    static_assert(std::is_implicit_lifetime_v<Vec4>);
#else
    static_assert(std::is_trivially_copyable_v<Vec4>);
#endif
    static_assert(std::is_standard_layout_v<Vec4>);

    inline constexpr Vec2::operator Vec3() const noexcept{
        return {x, y, 0.0f};
    }
    inline constexpr Vec2::operator Vec4() const noexcept{
        return {x, y, 0.0f, 0.0f};
    }

    inline constexpr Vec3::operator Vec4() const noexcept{
        return {x, y, z, 0.0f};
    }

    inline constexpr Vec3 toVec3(Vec2 v, f32 z=0.0f) noexcept{
        return {v.x, v.y, z};
    }
    inline constexpr Vec4 toVec4(Vec2 v, f32 z=0.0f, f32 w=0.0f) noexcept{
        return {v.x, v.y, z, w};
    }
    inline constexpr Vec4 toVec4(Vec3 v, f32 w=0.0f) noexcept{
        return {v.x, v.y, v.z, w};
    }

    inline constexpr Vec2 operator+(Vec2 lhs, Vec2 rhs) noexcept{
        return {lhs.x+rhs.x, lhs.y+rhs.y};
    }
    inline constexpr Vec2& operator+=(Vec2& lhs, Vec2 rhs) noexcept{
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator-(Vec2 v) noexcept{
        return {-v.x, -v.y};
    }
    inline constexpr Vec2 operator-(Vec2 lhs, Vec2 rhs) noexcept{
        return {lhs.x-rhs.x, lhs.y-rhs.y};
    }
    inline constexpr Vec2& operator-=(Vec2& lhs, Vec2 rhs) noexcept{
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(Vec2 lhs, Vec2 rhs) noexcept{
        return {lhs.x * rhs.x, lhs.y * rhs.y};
    }
    inline constexpr Vec2& operator*=(Vec2& lhs, Vec2 rhs) noexcept{
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        return lhs;
    }
    inline constexpr Vec2 operator*(f32 f, Vec2 v) noexcept{
        return {f * v.x, f * v.y};
    }
    inline constexpr Vec2 operator*(Vec2 v, f32 f) noexcept{
        return f * v;
    }
    inline constexpr Vec2& operator*=(Vec2& v, f32 f) noexcept{
        v.x *= f;
        v.y *= f;
        return v;
    }
    inline constexpr Vec2 operator/(Vec2 v, f32 f) noexcept{
        return {v.x/f, v.y/f};
    }
    inline constexpr Vec2& operator/=(Vec2& v, f32 f) noexcept{
        v.x /= f;
        v.y /= f;
        return v;
    }

    inline constexpr bool operator==(Vec2 lhs, Vec2 rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y;
    }

    inline constexpr Vec3 zeros() noexcept{
        return {0.0f, 0.0f, 0.0f};
    }
    inline constexpr Vec3 ones() noexcept{
        return {1.0f, 1.0f, 1.0f};
    }
    inline constexpr Vec3 unitX() noexcept{
        return {1.0f, 0.0f, 0.0f};
    }
    inline constexpr Vec3 unitY() noexcept{
        return {0.0f, 1.0f, 0.0f};
    }
    inline constexpr Vec3 unitZ() noexcept{
        return {0.0f, 0.0f, 1.0f};
    }

    inline constexpr Vec3 operator+(Vec3 lhs, Vec3 rhs) noexcept{
        return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z};
    }
    inline constexpr Vec3& operator+=(Vec3& lhs, Vec3 rhs) noexcept{
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator-(Vec3 v) noexcept{
        return {-v.x, -v.y, -v.z};
    }
    inline constexpr Vec3 operator-(Vec3 lhs, Vec3 rhs) noexcept{
        return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z};
    }
    inline constexpr Vec3& operator-=(Vec3& lhs, Vec3 rhs) noexcept{
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(Vec3 lhs, Vec3 rhs) noexcept{
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z
        };
    }
    inline constexpr Vec3& operator*=(Vec3& lhs, Vec3 rhs) noexcept{
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        return lhs;
    }
    inline constexpr Vec3 operator*(f32 f, Vec3 v) noexcept{
        return {
            f * v.x,
            f * v.y,
            f * v.z
        };
    }
    inline constexpr Vec3 operator*(Vec3 v, f32 f) noexcept{
        return f * v;
    }
    inline constexpr Vec3& operator*=(Vec3& v, f32 f) noexcept{
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return v;
    }
    inline constexpr Vec3 operator/(Vec3 v, f32 f) noexcept{
        return {
            v.x/f,
            v.y/f,
            v.z/f
        };
    }
    inline constexpr Vec3& operator/=(Vec3& v, f32 f) noexcept{
        v.x /= f;
        v.y /= f;
        v.z /= f;
        return v;
    }

    inline constexpr bool operator==(Vec3 lhs, Vec3 rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y && lhs.z==rhs.z;
    }

    inline constexpr Vec4 unitQuat() noexcept{
        return {0.0f, 0.0f, 0.0f, 1.0f};
    }

    inline constexpr Vec4 operator+(Vec4 lhs, Vec4 rhs) noexcept{
        return {lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z, lhs.w+rhs.w};
    }
    inline constexpr Vec4& operator+=(Vec4& lhs, Vec4 rhs) noexcept{
        lhs.x += rhs.x;
        lhs.y += rhs.y;
        lhs.z += rhs.z;
        lhs.w += rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator-(Vec4 v) noexcept{
        return {-v.x, -v.y, -v.z, -v.w};
    }
    inline constexpr Vec4 operator-(Vec4 lhs, Vec4 rhs) noexcept{
        return {lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z, lhs.w-rhs.w};
    }
    inline constexpr Vec4& operator-=(Vec4& lhs, Vec4 rhs) noexcept{
        lhs.x -= rhs.x;
        lhs.y -= rhs.y;
        lhs.z -= rhs.z;
        lhs.w -= rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator*(Vec4 lhs, Vec4 rhs) noexcept{
        return {
            lhs.x * rhs.x,
            lhs.y * rhs.y,
            lhs.z * rhs.z,
            lhs.w * rhs.w
        };
    }
    inline constexpr Vec4& operator*=(Vec4& lhs, Vec4 rhs) noexcept{
        lhs.x *= rhs.x;
        lhs.y *= rhs.y;
        lhs.z *= rhs.z;
        lhs.w *= rhs.w;
        return lhs;
    }
    inline constexpr Vec4 operator*(f32 f, Vec4 v) noexcept{
        return {
            f * v.x,
            f * v.y,
            f * v.z,
            f * v.w
        };
    }
    inline constexpr Vec4 operator*(Vec4 v, f32 f) noexcept{
        return f * v;
    }
    inline constexpr Vec4& operator*=(Vec4& v, f32 f) noexcept{
        v.x *= f;
        v.y *= f;
        v.z *= f;
        v.w *= f;
        return v;
    }
    inline constexpr Vec4 operator/(Vec4 v, f32 f) noexcept{
        return {
            v.x / f,
            v.y / f,
            v.z / f,
            v.w / f,
        };
    }
    inline constexpr Vec4& operator/=(Vec4& v, f32 f) noexcept{
        v.x /= f;
        v.y /= f;
        v.z /= f;
        v.w /= f;
        return v;
    }

    inline constexpr auto operator==(Vec4 lhs, Vec4 rhs) noexcept{
        return lhs.x==rhs.x && lhs.y==rhs.y &&
               lhs.z==rhs.z && lhs.w==rhs.w;
    }

    struct Size2D{
        u32 x = 0, y = 0;
    };

    struct Transform;

    struct Transform2D{
        Vec2 position;
        f32 theta;
        Vec2 scale;

        constexpr operator Transform() const noexcept;
    };

    inline constexpr Transform2D operator*(Transform2D lhs, Transform2D rhs) noexcept{
        return Transform2D{
            .position = lhs.position + rhs.position,
            .theta = lhs.theta + rhs.theta,
            .scale = lhs.scale * rhs.scale
        };
    }

    inline auto extractZRot(Vec4 quat) noexcept{
        f32 sinz_cosx = 2*(quat.w*quat.z + quat.x*quat.y);
        f32 cosz_cosx = 1 - 2*(quat.y*quat.y + quat.z*quat.z);

        return std::atan2(sinz_cosx, cosz_cosx);
    }

    struct Transform{
        Vec3 position;
        Vec4 rotation;
        Vec3 scale;

        // Defined in LinearAlgebra.hpp
        explicit constexpr operator Transform2D() const noexcept{
            return Transform2D{
                .position = static_cast<Vec2>(position),
                .theta = extractZRot(rotation),
                .scale = static_cast<Vec2>(scale)
            };
        }
    };
}

template<>
struct std::formatter<Smol::Vec2>: public std::formatter<std::string>{
    inline auto format(const Smol::Vec2& v, auto& ctx) const{
        return std::formatter<std::string>::format(
            std::format("({}, {})", v.x, v.y),
            ctx
        );
    }
};

template<>
struct std::formatter<Smol::Vec3>: public std::formatter<std::string>{
    inline auto format(const Smol::Vec3& v, auto& ctx) const{
        return std::formatter<std::string>::format(
            std::format("({}, {}, {})", v.x, v.y, v.z),
            ctx
        );
    }
};

template<>
struct std::formatter<Smol::Vec4>: public std::formatter<std::string>{
    inline auto format(const Smol::Vec4& v, auto& ctx) const{
        return std::formatter<std::string>::format(
            std::format("({}, {}, {}, {})", v.x, v.y, v.z, v.w),
            ctx
        );
    }
};
