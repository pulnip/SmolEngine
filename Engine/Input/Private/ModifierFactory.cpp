#include "DOM.hpp"
#include "ModifierFactory.hpp"
#include "NegateModifier.hpp"
#include "StringUtil.hpp"
#include "SwizzleModifier.hpp"

namespace Smol
{
    ModifierFactory createNegateModifierFactory(){
        return [](const DOM::Value& v){
            auto m = std::make_unique<NegateModifier>();

            // negate all for default
            m->negateX = v.get<bool>("negate_x")
                .value_or(true);
            m->negateY = v.get<bool>("negate_y")
                .value_or(true);
            m->negateZ = v.get<bool>("negate_z")
                .value_or(true);

            return m;
        };
    }

    inline SwizzleOrder toSwizzleOrder(StrView str){
        using enum SwizzleOrder;

        static StringHashMap<SwizzleOrder> map = {
            {"XYZ", XYZ},
            {"XZY", XZY},
            {"YXZ", YXZ},
            {"YZX", YZX},
            {"ZXY", ZXY},
            {"ZYX", ZYX}
        };

        auto upper = toUpper(str);
        if(auto it = map.find(upper); it != map.end()){
            return it->second;
        }

        return XYZ;
    }

    ModifierFactory createSwizzleModifierFactory(){
        return [](const DOM::Value& v){
            auto m = std::make_unique<SwizzleModifier>();

            auto order = v.get<Str>("order")
                .value_or("XYZ");
            m->order = toSwizzleOrder(order);

            return m;
        };
    }
}
