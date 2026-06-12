#pragma once

#include <functional>
#include <variant>
#include "Primitives.hpp"

namespace Smol
{
    // Defined at Lower layer
    struct UIContext;

    struct Checkbox{
        Str label;
        std::function<void(UIContext&, bool)> onChanged = [](UIContext&, bool){};
        bool v;

        void submit(UIContext&);
    };

    using Widget = std::variant<
        Checkbox
    >;
}
