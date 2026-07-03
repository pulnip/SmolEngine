#pragma once

#include "Primitives.hpp"

namespace Smol
{
    inline void hashCombine(usize& seed, usize v) {
        seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template<typename... Args>
    inline usize hashAll(const Args&... args) {
        usize h = 0;
        (hashCombine(h, std::hash<Args>{}(args)), ...);
        return h;
    }
}

template<>
struct std::hash<Smol::Vec2>{
    std::size_t operator()(const Smol::Vec2& v) const noexcept{
        using namespace Smol;

        return hashAll(v.x, v.y);
    }
};

template<>
struct std::hash<Smol::Vec3>{
    std::size_t operator()(const Smol::Vec3& v) const noexcept{
        using namespace Smol;

        return hashAll(v.x, v.y, v.z);
    }
};

template<>
struct std::hash<Smol::Vec4>{
    std::size_t operator()(const Smol::Vec4& v) const noexcept{
        using namespace Smol;

        return hashAll(v.x, v.y, v.z, v.w);
    }
};

template<>
struct std::hash<Smol::Size2D>{
    std::size_t operator()(const Smol::Size2D& s) const noexcept{
        using namespace Smol;

        return hashAll(s.x, s.y);
    }
};

template<>
struct std::hash<Smol::Size3D>{
    std::size_t operator()(const Smol::Size3D& s) const noexcept{
        using namespace Smol;

        return hashAll(s.x, s.y, s.z);
    }
};
