#pragma once

#include <type_traits>

namespace Smol
{
    template<typename T>
    concept EnumType = std::is_enum_v<T>;

    template<EnumType E>
    constexpr bool hasFlag(E flags, E test){
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) != 0;
    }

    template<EnumType E>
    constexpr bool hasAll(E flags, E test){
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) == static_cast<U>(test);
    }

    template<EnumType E, EnumType... Es>
    constexpr E combine(E first, Es... rest){
        using U = std::underlying_type_t<E>;
        return static_cast<E>((
            static_cast<U>(first) | ... | static_cast<U>(rest)
        ));
    }
}
