#pragma once

#include <vector>
#include "KeyCode.hpp"
#include "Primitives.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    using ActionName = Str;
    using KeyMappings = std::vector<KeyCode>;
    struct ActionInfo{
        KeyMappings mappings;
        // Counting for Action with Multiple Key mapping
        u32 count = 0;
    };
    using ActionMappings = StringHashMap<ActionInfo>;

    struct InputConfig{
        ActionMappings mappings;
    };
}
