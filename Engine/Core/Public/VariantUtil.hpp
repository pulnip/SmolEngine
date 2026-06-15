#pragma once

namespace Smol
{
    template<class... Ts>
    struct overload: Ts...{
        using Ts::operator()...;
    };
}
