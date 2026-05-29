#include "DOM.hpp"
#include "LogLocal.hpp"
#include "ModifierFactory.hpp"
#include "ModifierRegistry.hpp"

namespace Smol
{
    ModifierRegistry::ModifierRegistry()
        : factories({
            {"Negate", createNegateModifierFactory()},
            {"Swizzle", createSwizzleModifierFactory()}
        })
    {}

    ModifierRAII ModifierRegistry::Create(const DOM::Value& v){
        auto type = v.get<Str>("type");
        if(!type.has_value()){
            LOG_ERROR("key \"type\" not found in DOM::Value");
            return nullptr;
        }

        auto it = factories.find(*type);
        if(it == factories.end()){
            LOG_ERROR("Unknown InputModifier: {}", *type);
            return nullptr;
        }

        return it->second(v);
    };
}
