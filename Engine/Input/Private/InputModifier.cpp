#include "InputModifier.hpp"
#include "DOM.hpp"
#include "LogLocal.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    inline auto createNegateModifier(const DOM::Value& v){
        // negate all for default
        auto negateX = v.get<bool>("negate_x")
            .value_or(true);
        auto negateY = v.get<bool>("negate_y")
            .value_or(true);
        auto negateZ = v.get<bool>("negate_z")
            .value_or(true);

        return NegateModifier(negateX, negateY, negateZ);
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

        LOG_ERROR("Unknown SwizzleOrder: {}", str);
        return XYZ;
    }

    inline auto createSwizzleModifier(const DOM::Value& v){
        auto str = v.get<Str>("order")
            .value_or("XYZ");
        auto order = toSwizzleOrder(str);

        return SwizzleModifier(order);
    }

    InputModifier CreateInputModifier(const DOM::Value& v){
        auto type = v.get<Str>("type");
        if(!type.has_value()){
            LOG_ERROR("key \"type\" not found in DOM::Value");
            return NoModifier{};
        }

        if(*type == "Negate"){
            return createNegateModifier(v);
        }
        else if(*type == "Swizzle"){
            return createSwizzleModifier(v);
        }
        else{
            LOG_ERROR("Unknown InputModifier: {}", *type);
            return NoModifier{};
        }
    }
}
