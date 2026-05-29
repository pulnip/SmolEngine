#pragma once

#include <functional>
#include "CoreFWD.hpp"
#include "InputModifier.hpp"
#include "Semantics.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    namespace DOM{
        class Value;
    }

    using ModifierRAII = RAII<InputModifier>;
    using ModifierFactory = std::function<ModifierRAII(const DOM::Value&)>;

    class ModifierRegistry final{
    private:
        StringHashMap<ModifierFactory> factories;

    public:
        ModifierRegistry();
        ~ModifierRegistry() = default;
        SMOL_DECLARE_PINNED(ModifierRegistry)

        ModifierRAII Create(const DOM::Value&);
    };
}
