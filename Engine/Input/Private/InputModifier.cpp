#include "DOM.hpp"
#include "EnumUtil.hpp"
#include "InputModifier.hpp"
#include "LogLocal.hpp"
#include "StringUtil.hpp"

namespace Smol
{
    template<>
    CStr EnumTraits<SwizzleOrder>::name = "SwizzleOrder";

    template<>
    SwizzleOrder EnumTraits<SwizzleOrder>::convert(StrView str){
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
}

namespace{
    auto createNegateModifier(const Smol::DOM::Value& v){
        // negate all for default
        auto negateX = v.get<bool>("negate_x")
            .value_or(false);
        auto negateY = v.get<bool>("negate_y")
            .value_or(false);
        auto negateZ = v.get<bool>("negate_z")
            .value_or(false);

        auto anyNegate = negateX || negateY || negateZ;

        return anyNegate ?
            Smol::NegateModifier(negateX, negateY, negateZ) :
            // negate all for default
            Smol::NegateModifier(true, true, true);
    }

    auto createScaleModifier(const Smol::DOM::Value& v){
        using namespace Smol;

        if(auto factor = v.get<f32>("factor")){
            return ScaleModifier(*factor);
        }
        else if(auto factor = v.get<Vec3>("factor")){
            return ScaleModifier(*factor);
        }

        LOG_WARN("key \"factor\" not found in DOM::Value");
        return ScaleModifier();
    }

    auto createSwizzleModifier(const Smol::DOM::Value& v){
        using namespace Smol;

        if(auto order = v.get<Str>("order")){
            return SwizzleModifier(EnumTraits<SwizzleOrder>::convert(*order));
        }

        LOG_WARN("key \"order\" not found in DOM::Value");
        return SwizzleModifier();
    }
}

namespace Smol
{
    InputModifier CreateInputModifier(const DOM::Value& v){
        auto type = v.get<Str>("type");
        if(!type.has_value()){
            LOG_ERROR("key \"type\" not found in DOM::Value");
            return NoModifier{};
        }

        if(*type == "Negate"){
            return createNegateModifier(v);
        }
        else if(*type == "Scale"){
            return createScaleModifier(v);
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
