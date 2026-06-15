#pragma once

#include <variant>
#include <vector>
#include "InputModifier.hpp"
#include "KeyCode.hpp"
#include "Primitives.hpp"
#include "TomlLoader.hpp"

namespace Smol
{
    using ActionName = Str;
    struct KeyBinding{
        // fire condition
        std::variant<KeyCode, MouseButton> cond;
        std::vector<InputModifier> modifiers;
    };
    using KeyBindings = std::vector<KeyBinding>;
    struct ActionInfo{
        ActionName name;
        KeyBindings mappings;
        // Counting for Action with Multiple Key mapping
        u32 count = 0;
    };
    using ActionMappings = std::vector<ActionInfo>;

    struct InputConfig{
        ActionMappings mappings;
    };

    template<>
    struct TomlTraits<InputConfig>{
        static InputConfig from(const DOM::Value&, const TomlMetadata&);
    };
}
