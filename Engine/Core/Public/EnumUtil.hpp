#pragma once

#include <type_traits>

namespace Smol
{
    template<typename T>
    concept enum_type = std::is_enum_v<T>;

    template<enum_type E>
    constexpr bool has_flag(E flags, E test){
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) != 0;
    }

    template<enum_type E>
    constexpr bool has_all(E flags, E test){
        using U = std::underlying_type_t<E>;
        return (static_cast<U>(flags) & static_cast<U>(test)) == static_cast<U>(test);
    }

    template<enum_type E, enum_type... Es>
    constexpr E combine(E first, Es... rest){
        using U = std::underlying_type_t<E>;
        return static_cast<E>((
            static_cast<U>(first) | ... | static_cast<U>(rest)
        ));
    }
}
