#pragma once

#include <cstddef>
#include <cstdint>
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

    struct Vec3;
    struct Vec4;

    // column vector
    struct Vec2{
        float x = 0.0f, y = 0.0f;

        constexpr float& operator[](size_t i){
            switch(i){
            case 0: return x;
            case 1: return y;
            default:
                SMOL_ASSERT(false && "Vec2 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const{
            switch(i){
            case 0: return x;
            case 1: return y;
            default:
                SMOL_ASSERT(false && "Vec2 index out of range");
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
        float x = 0.0f, y = 0.0f, z = 0.0f;

        constexpr float& operator[](size_t i){
            switch(i){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                SMOL_ASSERT(false && "Vec3 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const{
            switch(i){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default:
                SMOL_ASSERT(false && "Vec3 index out of range");
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
        float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f;

        constexpr float& operator[](size_t i){
            switch(i){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:
                SMOL_ASSERT(false && "Vec4 index out of range");
                std::unreachable();
            }
        }
        constexpr const float& operator[](size_t i) const{
            switch(i){
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default:
                SMOL_ASSERT(false && "Vec4 index out of range");
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

    inline constexpr Vec3 to_vec3(Vec2 v, float z=0.0f){
        return {v.x, v.y, z};
    }
    inline constexpr Vec4 to_vec4(Vec2 v, float z=0.0f, float w=0.0f){
        return {v.x, v.y, z, w};
    }
    inline constexpr Vec4 to_vec4(Vec3 v, float w=0.0f){
        return {v.x, v.y, v.z, w};
    }
}
