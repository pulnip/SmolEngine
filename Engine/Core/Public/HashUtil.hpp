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
