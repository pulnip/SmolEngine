#pragma once

#include <cstddef>
#include <cstdint>
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
}
