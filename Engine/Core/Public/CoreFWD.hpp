#pragma once

#include <memory>

namespace Smol
{
    template<typename T>
    using RAII = std::unique_ptr<T>;

    class Object;
    using ObjectRAII = RAII<Object>;
}
