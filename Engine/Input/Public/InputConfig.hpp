#pragma once

#include <vector>
#include "KeyCode.hpp"
#include "Primitives.hpp"

namespace Smol
{
    using ActionName = Str;
    using KeyMappings = std::vector<KeyCode>;
    struct ActionInfo{
        ActionName name;
        KeyMappings mappings;
        // Counting for Action with Multiple Key mapping
        u32 count = 0;
    };
    using ActionMappings = std::vector<ActionInfo>;

    struct InputConfig{
        ActionMappings mappings;
    };
}
