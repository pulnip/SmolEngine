#pragma once

#include <memory>

namespace Smol
{
    template<typename T>
    using RAII = std::unique_ptr<T>;

    class Actor;
    using ActorRAII = RAII<Actor>;
}
