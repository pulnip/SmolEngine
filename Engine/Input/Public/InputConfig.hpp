#pragma once

#include "KeyCode.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    struct InputConfig{
        StringHashMap<KeyCode> mappings;
    };
}
